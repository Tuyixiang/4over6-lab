#include "msg.h"

void debug_print_msg(const struct Msg *msg) {
  static const char *type_names[5] = {
      "connection request", "connection response", "inbound traffic",
      "outbound traffic",   "heartbeat",
  };
  assert(msg->type >= MSG_IP_REQ && msg->type <= MSG_HEARTBEAT);
  LOG("\tlength:\t%d", msg->length);
  LOG("\ttype:\t%s", type_names[msg->type - MSG_IP_REQ]);
  LOG("\tdata:");
  printf("\x1b[2m");
  for (int i = 0; i < msg->length - 5; i++) {
    if (msg->data[i] >= 32 && msg->data[i] <= 127) {
      // visible character
      printf("%c", msg->data[i]);
    } else {
      printf("\\x%02x", msg->data[i]);
    }
  }
  printf("\x1b[0m\n");
}