#pragma once

int  main(int argc, char* argv[]);
int  validate_ip(char* ip);
int  traceroute_handle_step(int sockfd, int ttl, char* target_ip);
long compute_avarage(long* times);
void print_unique_addresses(long times[], char ip_addresses[3][20]);
