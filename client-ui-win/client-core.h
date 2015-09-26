#ifndef __CLIENTCORE_H
#define __CLIENTCORE_H

#ifdef CLIENTCORE_EXPORTS
#define CLIENTCORE_API __declspec(dllexport)
#else
#define CLIENTCORE_API __declspec(dllimport)
#endif

struct USER
{
	char name[100];
	char password[100];
	char nic[100];
//UI stop
//cmd start
	bool rundhcp = true;
	bool daemon = false;
	bool terminate = false;
	bool list = false;
	bool verbose = false;
	bool help = false;
};

#define ERROR_UNKNOWN -1
#define SUCCESS 0
#define ERROR_WRONG_NIC 1
#define ERROR_NO_PREMISSION 2
#define ERROR_OPENNIC_ERROR 3
#define ERROR_START_ERROR 4
#define ERROR_LOOP_BREAK 5
#define ERROR_READ_PACKET 6

#ifdef WIN32
CLIENTCORE_API int getnic(const char *netcard, char *nic);
#endif

CLIENTCORE_API int client_login(struct USER user
#ifdef WIN32
	, HWND hWnd
#endif
	);
CLIENTCORE_API int client_issuccess();
CLIENTCORE_API int client_test(struct USER user);
CLIENTCORE_API int client_logoff();
#endif
