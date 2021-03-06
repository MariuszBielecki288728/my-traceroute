// Mariusz Bielecki 288728

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "defines.h"
#include "icmp_receive.h"
#include "icmp_send.h"
#include "traceroute.h"

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
    for(int ttl = 1; ttl <= 30; ttl++)
    {
        printf("%d. ", ttl);
        status = traceroute_handle_step(sockfd, ttl, target_ip);

        if(status == TARGET_REACHED)
        {
            return EXIT_SUCCESS;
        }
    }

    EXIT_WITH_ERR("Couldn't trace %s\n", target_ip);
}

int validate_ip(char* ip)
{
    struct sockaddr_in sa;

    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    return result != 0;
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

    int reached = 0;

    long times[3];
    char sender_ip_str[3][20];
    memset(sender_ip_str, 0, sizeof(sender_ip_str[0][0]) * 3 * 20);
    int answered = 0;
    for(int i = 0; i < 3; i++)
    {
        int status = icmp_receive(sockfd, ttl, &tv, buffer, sender_ip_str[i]);
        if(status == TARGET_REACHED)
        {
            reached = 1;
        }
        if(status == TARGET_REACHED || status == EXCEEDED_ANSWER)
        {
            answered = 1;
            times[i] = tv.tv_usec || tv.tv_sec ? (1000000 - tv.tv_usec) / 1000 : 0;
        }
        else
        {
            times[i] = -1;
        }
    }
    if(answered)
    {
        long av = compute_avarage(times);
        print_unique_addresses(times, sender_ip_str);

        if(av == -1)
        {
            printf("???");
        }
        else
        {
            printf("%ldms", av);
        }
        printf("\n");
    }
    else
    {
        printf("*\n");
    }
    if(reached)
    {
        return TARGET_REACHED;
    }
    return TARGET_NOT_REACHED;
}

long compute_avarage(long* times)
{
    for(int i = 0; i < 3; i++)
    {
        if(times[i] == -1)
        {
            return -1;
        }
    }
    return (times[0] + times[1] + times[2]) / 3;
}

void print_unique_addresses(long times[], char ip_addresses[3][20])
{
    for(int j = 0; j < 3; j++)
    {
        if(times[j] != -1)
        {
            printf("%s ", ip_addresses[j]);
            for(int i = j; i < 3; i++)
            {
                if(!strcmp(ip_addresses[j], ip_addresses[i]))
                {
                    times[i] = -1;
                }
            }
        }
    }
}
