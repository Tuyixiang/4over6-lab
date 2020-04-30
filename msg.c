#include "msg.h"
#include <sys/time.h>

void debug_print_msg(const struct Msg *msg) {
  static const char *type_names[5] = {
      "connection request", "connection response", "inbound traffic",
      "outbound traffic",   "heartbeat",
  };
  assert(msg->type >= MSG_IP_REQ && msg->type <= MSG_HEARTBEAT);
  struct timeval tv;
  gettimeofday(&tv, NULL);
  LOG("\ttime:\t%llu", 1000000 * tv.tv_sec + tv.tv_usec);
  LOG("\ttype:\t%d %s", msg->type, type_names[msg->type - MSG_IP_REQ]);
}