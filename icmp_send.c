#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#include "icmp_checksum.c"

struct icmphdr prepare_icmp_header(uint16_t seq);

int icmp_send(char* target_ip)
{
    struct icmphdr icmp_header;
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (sockfd < 0)
    {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    

    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    inet_pton(AF_INET, target_ip, &recipient.sin_addr);

    int ttl = 1;
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
    for (uint16_t i = 0; i < 3; i++)
    {
        icmp_header = prepare_icmp_header(i);
        ssize_t bytes_sent = sendto(
            sockfd,
            &icmp_header,
            sizeof(icmp_header),
            0,
            (struct sockaddr *)&recipient,
            sizeof(recipient));
        if (bytes_sent < 0)
        {
            fprintf(stderr, "send error: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
    }
}

struct icmphdr prepare_icmp_header(uint16_t seq)
{
    pid_t pid = getpid();

    struct icmphdr icmp_header;
    icmp_header.type = ICMP_ECHO;
    icmp_header.code = 0;
    icmp_header.un.echo.id = htons((uint16_t)pid);
    icmp_header.un.echo.sequence = htons(seq);
    icmp_header.checksum = 0;
    icmp_header.checksum = compute_icmp_checksum(
        (u_int16_t *)&icmp_header,
        sizeof(icmp_header));

    return icmp_header;
}