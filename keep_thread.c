#include "keep_thread.h"

#include "main_thread.h"
#include "user_info.h"
#include "main.h"

void keep_thread() {
  SUCCESS("keepalive thread started");
  while (1) {
    time_t now = time(NULL);
    for (int i = 0; i < MAX_CLIENT; i++) {
      if (user_info_list[i].valid) {
        struct UserInfo *info = &user_info_list[i];
        pthread_mutex_lock(&info->lock);

        if (now - info->last_request >= DISCONNECT_TIMEOUT) {
          LOG("dropping " IP6_FMT, IP6(info->address_6));
          info->valid = 0;
        } else if (now - info->last_heartbeat >= HEARTBEAT_TIMEOUT) {
          LOG("sending heartbeat to " IP6_FMT, IP6(info->address_6));
          pthread_mutex_lock(&sock_server_lock);
          info->last_heartbeat = now;
          int len = sendto(sock_server, "\x05\x00\x00\x00\x68", 5, MSG_WAITALL,
                 (struct sockaddr *)&info->address_6,
                 sizeof(struct sockaddr_in6));
          if (len == -1) {
            LOG("errno: %d", errno);
          }
          pthread_mutex_unlock(&sock_server_lock);
        }

        pthread_mutex_unlock(&info->lock);
      }
    }
    sleep(2);
  }
}