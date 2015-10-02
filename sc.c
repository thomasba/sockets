#include "sc.h"
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

int fd = 0, ns = 0;

void close_fd()
{
	if ( fd != 0 )
	{
		close(fd);
	}
	if ( ns != 0 && ns != fd )
	{
		close(ns);
	}
}

void* thread_read(void * fdv)
{
	char c;
	while(1)
	{
		c = fgetc(stdin);
		if ( c == EOF )
		{
			close_fd();
			exit(0);
		}
		if( write( ns, &c, 1 ) != 1)
		{
			fprintf(stderr,"Error writing to socket!\n");
			close_fd();
			exit(2);
		}
	}
	close_fd();
	return NULL;
}

void* thread_write(void * fdv)
{
	char c;
	while( read( ns, &c, 1 ) == 1 && c != EOF )
	{
		putchar(c);
	}
	close_fd();
	exit(0);
	return NULL;
}

int main( int argc, char ** argv )
{
	struct sockaddr* saddr = NULL;
	int c = 0;
	options o = {AF_UNSPEC, NULL, NULL, 0, 0};
	pthread_t tw, tr;
	readOptions(argc, argv, &o);

	if ( o.type == AF_UNSPEC )
	{
		o.server = gethostbyname( o.address );
	}
	else
	{
		o.server = gethostbyname2( o.address, o.type );
	}
	if ( o.server == NULL )
	{
		fprintf(stderr, "Couldn’t resolve '%s'!\n", o.address);
		exit(2);
	}
	o.type = o.server->h_addrtype;


	if ( o.type == AF_INET )
	{
		open_socket_ipv4(&o, saddr);
	}
	else if ( o.type == AF_INET6 )
	{
		open_socket_ipv6(&o, saddr);
	}
	else
	{
		fprintf(stderr, "Unknown socket type!\n");
		return 2;
	}
	ns = fd;
	if ( o.listen == 1 )
	{
		c = sizeof(struct sockaddr_in);
		ns = accept(fd, saddr, (socklen_t*)&c);
		close(fd);
		fd = 0;
		printf("Listening on ");
	}
	else
	{
		printf("Connectiong to ");
	}
	print_ip_address( o.server->h_addr_list[0], o.type );
	printf(" on port %i ...\n", o.port);

	pthread_create(&tw, NULL, &thread_write, NULL);
	pthread_create(&tr, NULL, &thread_read, NULL);
	pthread_join(tw, NULL);
	pthread_join(tr, NULL);

	return 0;
}

void open_socket_ipv4( options * o, struct sockaddr* saddr )
{
	struct sockaddr_in *addr;
	addr = malloc( sizeof(struct sockaddr_in) );
	addr->sin_family = o->type;
	addr->sin_port   = htons(o->port);
	addr->sin_addr.s_addr = 0;
	memcpy( (char*)&addr->sin_addr.s_addr, (char*)o->server->h_addr, o->server->h_length  );
	open_socket(o, saddr, (struct sockaddr*) addr, sizeof(struct sockaddr_in) );
}

void open_socket_ipv6( options * o, struct sockaddr *saddr )
{
	struct sockaddr_in6 *addr;
	addr = malloc(sizeof(struct sockaddr));
	addr->sin6_family = o->type;
	addr->sin6_port   = htons(o->port);
	memset(addr->sin6_addr.s6_addr, 0, 16);
	memcpy( (char*)&addr->sin6_addr.s6_addr, (char*)o->server->h_addr, o->server->h_length  );
	open_socket(o, saddr, (struct sockaddr*) addr, sizeof(struct sockaddr_in6) );
}

void open_socket( options * o, struct sockaddr *saddr, struct sockaddr* addr, int l )
{
	int true = 1;
	fd = socket( o->type, SOCK_STREAM, 0 );
	if ( fd == 0 )
	{
		fprintf(stderr,"Can’t open socket. Bye!\n");
		exit(2);
	}
	if ( o->listen == 1 )
	{
		if ( bind(fd, addr, l) < 0 )
		{
			fprintf(stderr,"bind failed!\n");
			exit(2);
		}
		setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int));
		listen(fd, 5);
	}
	else
	{
		if ( connect(fd, addr, l) < 0 )
		{
			fprintf(stderr,"Error connecting. Bye!\n");
			exit(2);
		}
	}
	saddr = (struct sockaddr*)&addr;
}

void readOptions(int argc, char **argv, options * o)
{
	char c;
	while (( c = getopt (argc, argv, "46hl") ) != -1)
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
			case 'l':
				o->listen = 1;
				break;
			case '?':
				fprintf( stderr, "Unknown option `-%c'", optopt );
				print_usage(argc,argv);
				exit(1);
				break;
		}
	}
	if ( (argc-optind) != 2 )
	{
		print_usage(argc,argv);
		exit(1);
	}
	o->address = argv[optind];
	o->port    = atoi(argv[optind+1]);
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

void print_usage(int argc, char ** argv)
{
	printf("Usage:\n");
	printf("  %s [-4|-6] [-l] <hostname> <port>\n", argv[0]);
	printf("Options:\n");
	printf("  -4  Use IPv4\n");
	printf("  -6  Use IPv6\n");
	printf("  -l  Listen on given port\n");
}
