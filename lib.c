#include "types.h"

int open_socket_ipv4( options * o )
{
	int fd = NULL;
	struct sockaddr_in addr;
	addr.sin_family = o->type;
	addr.sin_port   = htons(o->port);
	addr.sin_addr.s_addr = 0;
	memcpy( (char*)&addr.sin_addr.s_addr, (char*)o->server->h_addr, o->server->h_length  );
	fd = socket( o->type, SOCK_STREAM, 0 );
	if ( fd == 0 )
	{
		fprintf(stderr,"Can’t open socket. Bye!\n");
		exit(2);
	}
	if ( connect(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0 )
	{
		fprintf(stderr,"Error connecting. Bye!\n");
		exit(2);
	}
	return fd;
}

int open_socket_ipv6( options * o )
{
	/* TODO */
	int fd = NULL;
	struct sockaddr_in6 addr;
	addr.sin6_family = o->type;
	addr.sin6_port   = htons(o->port);
	memset(addr.sin6_addr.s6_addr, 0, 8);
	memcpy( (char*)&addr.sin6_addr.s6_addr, (char*)o->server->h_addr, o->server->h_length  );
	fd = socket( o->type, SOCK_STREAM, 0 );
	if ( fd == 0 )
	{
		fprintf(stderr,"Can’t open socket. Bye!\n");
		exit(2);
	}
	if ( connect(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0 )
	{
		fprintf(stderr,"Error connecting. Bye!\n");
		exit(2);
	}
	return fd;
}



void print_ip_address( char * a, int type  )
{
	if ( type == AF_INET6 )
	{
		int i;
		for(i=0; i<7; i++)
		{
			printf("%02x%02x:", (unsigned char)a[2*i], (unsigned char)a[2*i+1]);
		}
		printf("%02x%02x", (unsigned char)a[14], (unsigned char)a[15]);
	}
	else if ( type == AF_INET)
	{
		printf("%u.%u.%u.%u", (unsigned char)a[0], (unsigned char)a[1], (unsigned char)a[2], (unsigned char)a[3]);
	}
	else
	{
		fprintf(stderr, "%s:%d#%s: Invalid type!", __FILE__, __LINE__, __func__);
	}
}
