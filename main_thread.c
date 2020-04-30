#include "main_thread.h"

#include "common.h"
#include "main.h"
#include "msg.h"

// epoll fd
int epfd;
struct epoll_event *events;

// 将 socket 加入 epoll，其中 user data 为 UserInfo *ptr（服务器连接为 NULL）
void add_socket(int sock, struct UserInfo *ptr) {
  struct epoll_event event;
  // user data
  event.data.ptr = ptr;
  // r/w, edge trigger
  event.events = EPOLLIN | EPOLLET;

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event) == -1) {
    ERR("epoll_ctl failed");
  }
}

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
    LOG("forward failed with errno: %d", errno);
  }
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
  epfd = epoll_create1(0);
  if (epfd < 0) {
    ERR("failed to create epoll fd");
  }
  // add IPv6 server socket
  add_socket(sock_server, NULL);

  // create event pool
  events = calloc(MAX_EPOLL_EVENT, sizeof(struct epoll_event));

  while (1) {
    int n_event = epoll_wait(epfd, events, MAX_EPOLL_EVENT, -1);
    for (int i = 0; i < n_event; i++) {
      struct epoll_event *event = &events[i];

      if (event->data.ptr == NULL) {
        // IPv6 socket incoming connection(s)
        while (1) {
          struct Msg msg;
          struct sockaddr_storage in_addr;
          socklen_t in_len = sizeof(struct sockaddr_storage);
          int len =
              recvfrom(sock_server, &msg, sizeof(struct Msg), MSG_DONTWAIT,
                       (struct sockaddr *)&in_addr, &in_len);
          if (len < 0) {
            // not ready or failed
            if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
              LOG("failed to accept");
            }
            break;
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

          info->last_request = time(NULL);

          switch (msg.type) {
            case MSG_IP_REQ:
              SUCCESS("received packet from " IP6_FMT ":",
                      IP6(info->address_6));
              debug_print_msg(&msg);
              handle_ip_req(info, &msg);
              break;
            case MSG_NET_REQ:
              SUCCESS("received packet from " IP6_FMT ":",
                      IP6(info->address_6));
              debug_print_msg(&msg);
              handle_net_req(info, &msg);
              break;
            case MSG_HEARTBEAT:
              SUCCESS("received packet from " IP6_FMT ":",
                      IP6(info->address_6));
              debug_print_msg(&msg);
              break;
            default:
              LOG("unexpected packet type: %d", msg.type);
          }

          pthread_mutex_unlock(&info->lock);
        }
      } else {
        ERR("wtf");
      }
    }
  }
}