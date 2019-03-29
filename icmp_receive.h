#pragma once

#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>

int icmp_receive(int sockfd, int ttl, struct timeval* timeout, uint8_t buffer[],
                 char sender_ip_str[]);
