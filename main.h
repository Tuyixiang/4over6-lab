#ifndef _MAIN_H_
#define _MAIN_H_

#include "user_info.h"

extern char *dns_string;
extern int tunfd;

extern int sock_server;
extern pthread_mutex_t sock_server_lock;

#endif  // _MAIN_H_