#pragma once

#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>

void print_as_bytes(unsigned char* buff, ssize_t length);
int icmp_receive(int sockfd, int ttl, struct timeval* timeout, uint8_t buffer[], char sender_ip_str[]);
