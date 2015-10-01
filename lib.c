#include <stdio.h>
#include <netdb.h>
#include "lib.h"

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
