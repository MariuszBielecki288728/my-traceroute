#include "icmp_receive.c"
#include "icmp_send.c"

#include <sys/time.h>

#define EXIT_WITH_ERR(err, ...)          \
    fprintf(stderr, err, ##__VA_ARGS__); \
    exit(EXIT_FAILURE);
#define TARGET_REACHED 1

int traceroute_handle_step(int sockfd, int ttl, char* target_ip);

int validate_ip(char* ip)
{
    struct sockaddr_in sa;

    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    return result != 0;
}

int main(int argc, char* argv[])
{
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sockfd < 0)
    {
        EXIT_WITH_ERR("socket error: %s\n", strerror(errno));
    }

    char* target_ip;
    if(argc < 0)
    {
        EXIT_WITH_ERR("argument error: ip address not provided");
    }
    target_ip = argv[1];

    if(!validate_ip(target_ip))
    {
        EXIT_WITH_ERR("argument error: provided ip address is invalid");
    }

    int status;
    for(int ttl = 1; ttl <= 40; ttl++)
    {
        status = traceroute_handle_step(sockfd, ttl, target_ip);

        if(status == TARGET_REACHED)
        {
            return EXIT_SUCCESS;
        }
    }

    EXIT_WITH_ERR("Couldn't trace %s", target_ip);
}

int traceroute_handle_step(int sockfd, int ttl, char* target_ip)
{
    for(int seq = 0; seq < 3; seq++)
    {
        icmp_send(sockfd, ttl, ttl, target_ip);
    }

    u_int8_t buffer[IP_MAXPACKET];

    struct timeval tv;
    tv.tv_sec  = 1;
    tv.tv_usec = 0;

    for(int i = 0; i < 3; i++)
    {
        icmp_receive(sockfd, ttl, &tv, buffer);
        //struct iphdr* ip_header = (struct iphdr*)buffer;
    }
    return 0;
}