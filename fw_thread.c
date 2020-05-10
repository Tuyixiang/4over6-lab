#include "fw_thread.h"

#include "header.h"
#include "main.h"
#include "main_thread.h"
#include "msg.h"
#include "user_info.h"

void fw_thread_once() {
  struct Msg msg;
  int len = read(tunfd, msg.data, sizeof(msg.data));
  if (len < 1) {
    return;
  }
  uint32_t dst_ip = get_dst_ip(msg.data);
  int offset = dst_ip - BASE_IP;
  if (offset < 0 || offset >= MAX_CLIENT) {
    LOG("received invalid tun packet");
    return;
  }
  struct UserInfo *info = &user_info_list[offset];
  if (!info->valid) {
    LOG("received tun packet for " IP4_FMT " but no registry found",
        IP4(info->address_4));
    return;
  }

#ifdef LOSSY
  if (rand() & 7 == 0) {
    // loss
    return;
  }
#endif

  SUCCESS("receiving tun packet for " IP4_FMT "(" IP6_FMT ")",
          IP4(info->address_4), IP6(info->address_6));

  msg.length = len + 5;
  msg.type = MSG_NET_RES;

  len =
      sendto(sock_server, &msg, msg.length, MSG_WAITALL,
             (struct sockaddr *)&info->address_6, sizeof(struct sockaddr_in6));
  if (len < 0) {
    LOG("forward failed with errno: %d", errno);
  }
}