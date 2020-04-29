#ifndef _MSG_H_
#define _MSG_H_

#include "common.h"

struct Msg {
  int length;
  char type;
  char data[4096];
};

#define MSG_IP_REQ 100
#define MSG_IP_RES 101
#define MSG_NET_REQ 102
#define MSG_NET_RES 103
#define MSG_HEARTBEAT 104

void debug_print_msg(const struct Msg *);

#endif  // _MSG_H_