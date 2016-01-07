#include "stdafx.h"
#include "ws2_32.h"
// #include <ws2tcpip.h>

EXPORT_DLL void freeaddrinfo(struct addrinfo *ai)
{

}

EXPORT_DLL int getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res)
{
	return 1;
}

