
#pragma once

// #ifdef DLL_IMPLEMENT
// __declspec(dllexport)
// #else
// __declspec(dllimport)
// #endif
#define EXPORT_DLL __declspec(dllexport)

EXPORT_DLL void freeaddrinfo(struct addrinfo *ai);
EXPORT_DLL int getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res);