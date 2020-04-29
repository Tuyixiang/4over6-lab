#ifndef _COMMON_H_

#define LISTEN_PORT 8080
#define MAX_LISTEN_QUEUE 4
#define MAX_CLIENT 128
#define NET_PORT 30000
#define MAX_EPOLL_EVENT 64

#include <assert.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#define ERR(format, ...)                            \
  do {                                              \
    printf("\x1b[31m%s:%d:\t", __FILE__, __LINE__); \
    printf(format, ##__VA_ARGS__);                  \
    printf("\x1b[0m\n");                            \
    exit(-1);                                       \
  } while (0);

#define SUCCESS(format, ...)                        \
  do {                                              \
    printf("\x1b[32m%s:%d:\t", __FILE__, __LINE__); \
    printf(format, ##__VA_ARGS__);                  \
    printf("\x1b[0m\n");                            \
  } while (0);

#define LOG(format, ...)                    \
  do {                                      \
    printf("%s:%d:\t", __FILE__, __LINE__); \
    printf(format, ##__VA_ARGS__);          \
    printf("\n");                           \
  } while (0);

#endif  // _COMMON_H_
