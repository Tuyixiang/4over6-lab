#include "main_thread.h"

#include "common.h"
#include "main.h"
#include "msg.h"

void handle_ip_req(struct UserInfo *info, struct Msg *msg) {
  int len = sprintf(msg->data, IP4_FMT " 0.0.0.0 %s", IP4(info->address_4),
                    dns_string);
  msg->length = len + 5;
  msg->type = MSG_IP_RES;

  pthread_mutex_lock(&sock_server_lock);
  len =
      sendto(sock_server, msg, msg->length, MSG_WAITALL,
             (struct sockaddr *)&info->address_6, sizeof(struct sockaddr_in6));
  if (len < 0) {
    WARN("forward failed with errno: %d", errno);
  }
  LOG("send %s", msg->data);
  pthread_mutex_unlock(&sock_server_lock);
}

void handle_net_req(struct UserInfo *info, struct Msg *msg) {
  int s = write(tunfd, msg->data, msg->length - 5);
  if (s < 0) {
    ERR("failed to write tun");
  }
  LOG("%d bytes sent to tun", s);
}

void main_thread() {
  SUCCESS("main thread started");

  while (1) {
    struct Msg msg;
    struct sockaddr_storage in_addr;
    socklen_t in_len = sizeof(struct sockaddr_storage);
    int len = recvfrom(sock_server, &msg, sizeof(struct Msg), MSG_WAITALL,
                       (struct sockaddr *)&in_addr, &in_len);
    if (len < 0) {
      // not ready or failed
      if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
        WARN("failed to accept");
      }
      continue;
    }

    if (len < 5) {
      LOG("incomplete packet: %d", len);
      continue;
    } else if (len != msg.length) {
      LOG("incomplete packet: %d, expecting %d", len, msg.length);
      continue;
    }

    if (in_addr.ss_family != AF_INET6) {
      LOG("non-ipv6 connection");
      continue;
    }

    // get user info
    struct UserInfo *info =
        get_locked_user_info_slot((struct sockaddr_in6 *)&in_addr);

    if (info == NULL) {
      WARN("user list full, dropping connection from " IP6_FMT,
           IP6(*(struct sockaddr_in6 *)&in_addr));
      continue;
    }

    info->last_request = time(NULL);

    switch (msg.type) {
      case MSG_IP_REQ:
      case MSG_NET_REQ:
      case MSG_HEARTBEAT:
        SUCCESS("received packet from " IP6_FMT "(" IP4_FMT "):",
                IP6(info->address_6), IP4(info->address_4));
        debug_print_msg(&msg);
        break;
      case MSG_DISCONNECT:
        SUCCESS("disconnect from " IP6_FMT, IP6(info->address_6));
        pthread_mutex_unlock(&info->lock);
        free_user_info(info);
        continue;
      case MSG_RESET:
        SUCCESS("RESET signal from " IP6_FMT, IP6(info->address_6));
        pthread_mutex_unlock(&info->lock);
        for (int i = 0; i < MAX_CLIENT; i++) {
          if (user_info_list[i].valid) {
            free_user_info(&user_info_list[i]);
          }
        }
        continue;
      default:
        LOG("unexpected packet type: %d", msg.type);
    }

    switch (msg.type) {
      case MSG_IP_REQ:
        handle_ip_req(info, &msg);
        break;
      case MSG_NET_REQ:
        handle_net_req(info, &msg);
        break;
    }

    pthread_mutex_unlock(&info->lock);
  }
}