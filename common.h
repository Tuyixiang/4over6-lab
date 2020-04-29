#ifndef _COMMON_H_

#define LISTEN_PORT 8888
#define MAX_LISTEN_QUEUE 4
#define MAX_CLIENT 128
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

#define IP4_FMT "%d.%d.%d.%d"
#define IP6_FMT "%x:%x:%x:%x:%x:%x:%x:%x"
#define IP4(addr) \
  (addr) >> 24, ((addr) >> 16) & 255, ((addr) >> 8) & 255, (addr)&255
#define IP6(addr)                                                   \
  (addr).__in6_u.__u6_addr16[0], (addr).__in6_u.__u6_addr16[1],     \
      (addr).__in6_u.__u6_addr16[2], (addr).__in6_u.__u6_addr16[3], \
      (addr).__in6_u.__u6_addr16[4], (addr).__in6_u.__u6_addr16[5], \
      (addr).__in6_u.__u6_addr16[6], (addr).__in6_u.__u6_addr16[7]

#endif  // _COMMON_H_
