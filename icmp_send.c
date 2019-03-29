// Mariusz Bielecki 288728

#include <errno.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "defines.h"
#include "icmp_checksum.h"
#include "icmp_send.h"

int icmp_send(int sockfd, int ttl, int seq, char* target_ip)
{
    struct icmphdr icmp_header;

    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    inet_pton(AF_INET, target_ip, &recipient.sin_addr);

    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
    icmp_header = prepare_icmp_header(seq);

    ssize_t bytes_sent = sendto(sockfd, &icmp_header, sizeof(icmp_header), 0,
                                (struct sockaddr*)&recipient, sizeof(recipient));
    if(bytes_sent < 0)
    {
        EXIT_WITH_ERR("send error: %s\n", strerror(errno));
    }
    return EXIT_SUCCESS;
}

struct icmphdr prepare_icmp_header(uint16_t seq)
{
    pid_t pid = getpid();

    struct icmphdr icmp_header;
    icmp_header.type             = ICMP_ECHO;
    icmp_header.code             = 0;
    icmp_header.un.echo.id       = htons((uint16_t)pid);
    icmp_header.un.echo.sequence = htons(seq);
    icmp_header.checksum         = 0;

    icmp_header.checksum =
    compute_icmp_checksum((u_int16_t*)&icmp_header, sizeof(icmp_header));

    return icmp_header;
}