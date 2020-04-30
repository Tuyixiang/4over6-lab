#include "main.h"

#include <linux/if.h>
#include <linux/if_tun.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include "fw_thread.h"
#include "keep_thread.h"
#include "main_thread.h"

char *dns_string;
int tunfd;
// IPv6 监听的接口
int sock_server;
pthread_mutex_t sock_server_lock;

void get_dns() {
  system("cat /etc/resolv.conf | grep -i nameserver | cut -c 12-30 > dns.txt");
  FILE *file = fopen("dns.txt", "r");
  char buf[1024];
  char dns[3][100];

  int len = fread(buf, 1, 1024, file);
  if (len < 0) {
    ERR("cannot read 'dns.txt'")
  }

  dns[0][0] = dns[1][0] = dns[2][0] = buf[len] = '\0';
  int dns_count = sscanf(buf, "%s %s %s", dns[0], dns[1], dns[2]);
  if (dns_count < 1) {
    ERR("failed to get dns");
  }

  asprintf(&dns_string, "%s %s %s", dns[0], dns[1], dns[2]);

  SUCCESS("get dns address: %s", dns_string);
}

void open_tun() {
  char tun_name[] = "tun4o6";
  tunfd = open("/dev/net/tun", O_RDWR | O_CREAT);
  if (tunfd == -1) {
    ERR("failed to open tun");
  }
  // set flags
  struct ifreq ifr;
  memset(&ifr, 0, sizeof(struct ifreq));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
  // set name
  strcpy(ifr.ifr_name, tun_name);
  // apply
  if (ioctl(tunfd, TUNSETIFF, &ifr) < 0) {
    ERR("failed to configure tun");
  }

  char buffer[128];
  sprintf(buffer, "ip link set dev %s up", tun_name);
  system(buffer);
  sprintf(buffer, "ip a add 13.8.0.1/24 dev %s", tun_name);
  system(buffer);
  sprintf(buffer, "ip link set dev %s mtu 1500", tun_name);
  system(buffer);
}

// 创建并返回 IPv6 接口
void create_ipv6_socket() {
  pthread_mutex_init(&sock_server_lock, NULL);
  struct sockaddr_in6 address;
  // create socket
  sock_server = socket(AF_INET6, SOCK_DGRAM, 0);
  if (sock_server < 0) {
    ERR("failed to create socket");
  }
  // bind to port
  address.sin6_addr = in6addr_any;
  address.sin6_family = AF_INET6;
  address.sin6_port = htons(LISTEN_PORT);
  bind(sock_server, (struct sockaddr *)&address, sizeof(address));

  SUCCESS("listening on port %d", LISTEN_PORT);
}

int main() {
  get_dns();
  open_tun();
  create_ipv6_socket();
  init_user_info_list();

  if (fork() == 0) {
    fw_thread();
  } else if (fork() == 0) {
    keep_thread();
  } else {
    main_thread();
  }
  return 0;
}