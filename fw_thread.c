#include "fw_thread.h"

#include "header.h"
#include "main.h"
#include "user_info.h"

void *fw_thread(void *_data) {
  char buffer[RECV_BUFFER_LENGTH];
  while (1) {
    int len = recv(tunfd, buffer, RECV_BUFFER_LENGTH, MSG_WAITALL);
    if (len < 0) {
      if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
        ERR("failed to read from tun");
      }
    } else {
      uint32_t dst_ip = get_dst_ip(buffer);
      int offset = dst_ip - BASE_IP;
      assert(offset >= 0 && offset < MAX_CLIENT);
      struct UserInfo *info = &user_info_list[offset];
      LOG("receiving tun packet for " IP4_FMT "(" IP6_FMT ")", IP4(info->address_4), IP6(info->address_6));
    }
  }
}