#ifndef _HEADER_H_
#define _HEADER_H_

#include "common.h"

static in_addr_t get_dst_ip(void *header) {
  unsigned char *dst_ip = header + 16;
  return (dst_ip[0] << 24) | (dst_ip[1] << 16) | (dst_ip[2] << 8) | dst_ip[3];
}

#endif  // _HEADER_H_