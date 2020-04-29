#include "main_thread.h"

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/epoll.h>

#include "main.h"
#include "msg.h"

// IPv6 监听的接口
int server_sock;
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
  server_sock = create_ipv6_socket();
  add_socket(server_sock, NULL);

  // create event pool
  events = calloc(MAX_EPOLL_EVENT, sizeof(struct epoll_event));

  while (1) {
    int n_event = epoll_wait(epfd, events, MAX_EPOLL_EVENT, -1);
    for (int i = 0; i < n_event; i++) {
      struct epoll_event *event = &events[i];

      if (event->data.ptr == NULL) {
        // IPv6 socket incoming connection(s)
        while (1) {
          int in_sock = accept(server_sock, NULL, NULL);
          if (in_sock == -1) {
            // not ready or failed
            if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
              LOG("failed to accept");
            } else {
              LOG("accept not ready");
            }
            break;
          }
          SUCCESS("accept");

        }
      }
    }
  }
}