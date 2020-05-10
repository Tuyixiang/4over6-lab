#include "keep_thread.h"

#include "main.h"
#include "main_thread.h"
#include "user_info.h"

void keep_thread_once() {
  time_t now = time(NULL);
  for (int i = 0; i < MAX_CLIENT; i++) {
    if (user_info_list[i].valid) {
      struct UserInfo *info = &user_info_list[i];

      if (now - info->last_request >= DISCONNECT_TIMEOUT) {
        LOG("dropping " IP6_FMT, IP6(info->address_6));
        info->valid = 0;
      } else if (now - info->last_heartbeat >= HEARTBEAT_TIMEOUT) {
        LOG("sending heartbeat to " IP6_FMT, IP6(info->address_6));
        info->last_heartbeat = now;
        int len = sendto(sock_server, "\x05\x00\x00\x00\x68", 5, MSG_WAITALL,
                         (struct sockaddr *)&info->address_6,
                         sizeof(struct sockaddr_in6));
        if (len == -1) {
          WARN("failed to send heartbeat: %d", errno);
        }
      }

    }
  }
}