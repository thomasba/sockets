#ifndef _ccc37729_ab1d_4386_82d3_201a093f27ff_types_h
#define _ccc37729_ab1d_4386_82d3_201a093f27ff_types_h

#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>


typedef struct options {
	int type;
	char * address;
	struct hostent * server;
	int port;
} options;

void print_ip_address( char * a, int type  );
int open_socket_ipv4(options * o);
int open_socket_ipv6(options * o);

#endif
