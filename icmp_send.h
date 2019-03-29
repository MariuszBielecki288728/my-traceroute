#pragma once

#include <arpa/inet.h>

int icmp_send(int sockfd, int ttl, int seq, char *target_ip);
struct icmphdr prepare_icmp_header(uint16_t seq);
