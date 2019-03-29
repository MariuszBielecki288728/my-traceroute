#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "defines.h"
#include "icmp_receive.h"

void print_as_bytes(unsigned char* buff, ssize_t length)
{
    for(ssize_t i = 0; i < length; i++, buff++)
        printf("%.2x ", *buff);
}

int icmp_receive(int sockfd, int ttl, struct timeval* timeout, uint8_t buffer[], char sender_ip_str[])
{
    for(;;)
    {
        struct sockaddr_in sender;
        socklen_t          sender_len = sizeof(sender);

        fd_set descriptors;
        FD_ZERO(&descriptors);
        FD_SET(sockfd, &descriptors);
        int ready = select(sockfd + 1, &descriptors, NULL, NULL, timeout);
        if(ready < 0)
        {
            EXIT_WITH_ERR("select error: %s\n", strerror(errno));
        }
        else if(ready == 0)
        {
            return NO_ANSWER;
        }


        ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0,
                                      (struct sockaddr*)&sender, &sender_len);
        if(packet_len < 0)
        {
            EXIT_WITH_ERR("recvfrom error: %s\n", strerror(errno));
        }

        
        inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, 20);

        struct iphdr*   ip_header     = (struct iphdr*)buffer;
        ssize_t         ip_header_len = 4 * ip_header->ihl;
        struct icmphdr* icmp_header = (struct icmphdr*)(buffer + ip_header_len);
        if(icmp_header->type == ICMP_TIME_EXCEEDED)
        {
            uint8_t* beg_of_orig_datagram = buffer + ip_header_len + sizeof(icmp_header);
            ip_header                  = (struct iphdr*)(beg_of_orig_datagram);
            ip_header_len              = 4 * ip_header->ihl;
            icmp_header = (struct icmphdr*)(beg_of_orig_datagram + ip_header_len);
        }
        if(icmp_header->un.echo.id != htons(getpid()) ||
           icmp_header->un.echo.sequence != htons(ttl))
        {
            continue;
        }

        if(icmp_header->type == ICMP_ECHOREPLY)
        {
            return TARGET_REACHED;
        }
        return EXCEEDED_ANSWER;
    }

    return TARGET_NOT_REACHED;
}
