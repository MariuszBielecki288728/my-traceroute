#pragma once

int  main(int argc, char* argv[]);
int  traceroute_handle_step(int sockfd, int ttl, char* target_ip);
int  validate_ip(char* ip);
long compute_avarage(long* times);