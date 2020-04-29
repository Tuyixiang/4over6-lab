#include "main_thread.h"

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "main.h"
#include "msg.h"

// IPv6 监听的接口
int sock_server;
// epoll fd
int epfd;
struct epoll_event *events;

void set_non_blocking(int sock) {
  int flags;

  flags = fcntl(sock, F_GETFL, 0);
  if (flags == -1) {
    ERR("failed to get socket flags");
  }

  flags |= O_NONBLOCK;
  if (fcntl(sock, F_SETFL, flags) == -1) {
    ERR("failed to set non-blocking flag");
  }
}

// 创建并返回 IPv6 接口
int create_ipv6_socket() {
  int sock;
  struct sockaddr_in6 address;
  // create socket
  sock = socket(AF_INET6, SOCK_STREAM, 0);
  if (sock < 0) {
    ERR("failed to create socket");
  }
  // bind to port
  address.sin6_addr = in6addr_any;
  address.sin6_family = AF_INET6;
  address.sin6_port = htons(LISTEN_PORT);
  bind(sock, (struct sockaddr *)&address, sizeof(address));
  // non-blocking
  set_non_blocking(sock);
  // start listening
  listen(sock, MAX_LISTEN_QUEUE);

  SUCCESS("listening on IPv6");
  return sock;
}

// 将 socket 加入 epoll，其中 user data 为 UserInfo *ptr（服务器连接为 NULL）
void add_socket(int sock, struct UserInfo *ptr) {
  struct epoll_event event;
  // user data
  event.data.ptr = ptr;
  // r/w, edge trigger
  event.events = EPOLLIN | EPOLLOUT | EPOLLET;

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event) == -1) {
    ERR("epoll_ctl failed");
  }
}

void main_thread() {
  epfd = epoll_create1(0);
  if (epfd < 0) {
    ERR("failed to create epoll fd");
  }
  // add IPv6 server socket
  sock_server = create_ipv6_socket();
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
          struct sockaddr_storage in_addr;
          socklen_t in_len = sizeof(struct sockaddr);
          int sock_in =
              accept(sock_server, (struct sockaddr *)&in_addr, &in_len);

          if (sock_in == -1) {
            // not ready or failed
            if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
              LOG("failed to accept");
            }
            break;
          }

          // drop if full
          if (user_info_list_full()) {
            LOG("client full, dropping connection");
            close(sock_in);
            continue;
          }

          SUCCESS("accept");

          // get client info
          if (in_addr.ss_family == AF_INET6) {
            struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)&in_addr;

            // create user info
            struct UserInfo *user_info = get_locked_user_info_slot();

            // save address6 & sock_in
            user_info->address_6 = addr6->sin6_addr;
            user_info->sock_in = sock_in;

            pthread_mutex_unlock(&user_info->lock);

            SUCCESS("user info created");

            debug_print(user_info);
            
          } else {
            LOG("get ipv4 connection, dropping");
            close(sock_in);
          }
        }
      }
    }
  }
}