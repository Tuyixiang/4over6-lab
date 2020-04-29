#ifndef _COMMON_H_
#define _COMMON_H_

#define LISTEN_PORT 8888
#define MAX_LISTEN_QUEUE 4
#define MAX_CLIENT 128
#define MAX_EPOLL_EVENT 64
#define SHARED_MEMORY "/4over6_lab_shm"
#define BASE_IP 0x0D080002U
#define RECV_BUFFER_LENGTH 8192

#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

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
#define IP4(addr)                                   \
  (addr).s_addr >> 24, ((addr).s_addr >> 16) & 255, \
      ((addr).s_addr >> 8) & 255, (addr).s_addr & 255
#define IP6(addr)                                                          \
  (addr).__in6_u.__u6_addr8[0] * 256 + (addr).__in6_u.__u6_addr8[1],       \
      (addr).__in6_u.__u6_addr8[2] * 256 + (addr).__in6_u.__u6_addr8[3],   \
      (addr).__in6_u.__u6_addr8[4] * 256 + (addr).__in6_u.__u6_addr8[5],   \
      (addr).__in6_u.__u6_addr8[6] * 256 + (addr).__in6_u.__u6_addr8[7],   \
      (addr).__in6_u.__u6_addr8[8] * 256 + (addr).__in6_u.__u6_addr8[9],   \
      (addr).__in6_u.__u6_addr8[10] * 256 + (addr).__in6_u.__u6_addr8[11], \
      (addr).__in6_u.__u6_addr8[12] * 256 + (addr).__in6_u.__u6_addr8[13], \
      (addr).__in6_u.__u6_addr8[14] * 256 + (addr).__in6_u.__u6_addr8[15]

#endif  // _COMMON_H_
