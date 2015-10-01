#ifndef _dec4c0b1_5d5e_42b6_8728_280329786b32_client_h
#define _dec4c0b1_5d5e_42b6_8728_280329786b32_client_h

typedef struct options {
	int type;
	char * address;
	struct hostent * server;
	int port;
} options;

void print_usage(int argc, char ** argv);
void readOptions(int argc, char **argv, options * o);
int open_socket_ipv4(options * o);
int open_socket_ipv6(options * o);

#endif
