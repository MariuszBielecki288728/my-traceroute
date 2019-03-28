#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define EXIT_WITH_ERR(err, ...)          \
    fprintf(stderr, err, ##__VA_ARGS__); \
    exit(EXIT_FAILURE);
#define EXIT_TIMEOUT 1

void print_as_bytes(unsigned char* buff, ssize_t length)
{
    for(ssize_t i = 0; i < length; i++, buff++)
        printf("%.2x ", *buff);
}

int icmp_receive(int sockfd, int ttl, struct timeval* timeout, uint8_t buffer[])
{
    for(;;)
    {
        struct sockaddr_in sender;
        socklen_t          sender_len = sizeof(sender);

        fd_set descriptors;
        FD_ZERO(&descriptors);
        FD_SET(sockfd, &descriptors);
        int ready = select(sockfd + 1, &descriptors, NULL, NULL, timeout);
        printf("%d\n", ready);
        if(ready < 0)
        {
            EXIT_WITH_ERR("select error: %s\n", strerror(errno));
        }
        else if(ready == 0)
        {
            return EXIT_TIMEOUT;
        }


        ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0,
                                      (struct sockaddr*)&sender, &sender_len);
        if(packet_len < 0)
        {
            EXIT_WITH_ERR("recvfrom error: %s\n", strerror(errno));
        }

        char sender_ip_str[20];
        inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));

        struct iphdr*   ip_header     = (struct iphdr*)buffer;
        ssize_t         ip_header_len = 4 * ip_header->ihl;
        struct icmphdr* icmp_header = (struct icmphdr*)(buffer + ip_header_len);

        if(icmp_header->un.echo.id != htons(getpid()) || icmp_header->un.echo.sequence != htons(ttl))
        {
            continue;
        }

        printf("Received IP packet with ICMP content from: %s\n", sender_ip_str);

        printf("IP header: ");
        print_as_bytes(buffer, ip_header_len);
        printf("\n");

        printf("IP data:   ");
        print_as_bytes(buffer + ip_header_len, packet_len - ip_header_len);
        printf("\n\n");
        break;
    }

    return EXIT_SUCCESS;
}
