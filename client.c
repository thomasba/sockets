#include "client.h"

int main( int argc, char ** argv )
{
	int fd = 0;
	char c;
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
	else
	{
		fprintf(stderr, "Unknown socket type!\n");
		return 2;
	}

	while(1) {
		c = fgetc(stdin);
		if ( c == EOF )
			break;
		if( write( fd, &c, 1 ) != 1)
		{
			fprintf(stderr,"Error writing to socket!\n");
			close(fd);
			return 2;
		}
	}
	close(fd);

	return 0;
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
