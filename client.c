#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef struct options {
	int type;
	char * address;
	struct hostent * server;
	int port;
} options;

/***************/
void print_ip_address( char * a, int type  );
void print_usage(int argc, char ** argv);
void readOptions(int argc, char **argv, options * o);
int open_socket_ipv4(options * o);
int open_socket_ipv6(options * o);

/***************/
int main( int argc, char ** argv )
{
	int fd = 0;
	options o = {AF_UNSPEC, NULL, NULL, 0};
	readOptions(argc, argv, &o);
	if ( o.type == AF_UNSPEC )
	{
		o.server = gethostbyname( o.address );
	}
	else
	{
		o.server = gethostbyname2( o.address, o.type );
	}
	if ( o.server == NULL ) {
		fprintf(stderr, "Couldn’t resolve '%s'!\n", o.address);
		exit(2);
	}
	o.type = o.server->h_addrtype;
	printf("Connecting to [");
	print_ip_address( o.server->h_addr_list[0], o.type );
	printf("]:%i ...\n", o.port);

	if ( o.type == AF_INET )
	{
		fd = open_socket_ipv4(&o);
	}
	else if ( o.type == AF_INET6 )
	{
		fd = open_socket_ipv6(&o);
	}

	write(fd, "test\n", strlen("test\n"));
	close(fd);

	return 0;
}

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
void readOptions(int argc, char **argv, options * o)
{
	char c;
	while (( c = getopt (argc, argv, "46h") ) != -1)
	{
		switch(c)
		{
			case '4':
				o->type = AF_INET;
				break;
			case '6':
				o->type = AF_INET6;
				break;
			case 'h':
				print_usage(argc,argv);
				exit(0);
			case '?':
				fprintf( stderr, "Unknown option `-%c'", optopt );
				print_usage(argc,argv);
				exit(1);
				break;
		}
	}
	if ( (argc-optind) != 2 ) {
		print_usage(argc,argv);
		exit(1);
	}
	o->address = argv[optind];
	o->port    = atoi(argv[optind+1]);
}

void print_usage(int argc, char ** argv)
{
	printf("Usage:\n");
	printf("  %s [-4|-6] <hostname> <port>\n", argv[0]);
}
