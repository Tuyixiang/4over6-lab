#include "main.h"

#include "main_thread.h"
#include <stdio.h>
#include <linux/if.h>
#include <linux/if_tun.h>

char *dns_string;
int tunfd;

void get_dns() {
  system("cat /etc/resolv.conf | grep -i nameserver | cut -c 12-30 > dns.txt");
  FILE *file = fopen("dns.txt", "r");
  char buf[1024];
  char dns[3][100];

  if (fread(buf, 1, 1024, file) < 0) {
    ERR("cannot read 'dns.txt'")
  }
  
  dns[0][0] = dns[1][0] = dns[2][0] = '\0';
  int dns_count = sscanf(buf, "%s %s %s", dns[0], dns[1], dns[2]);

  asprintf(&dns_string, "%s %s %s", dns[0], dns[1], dns[2]);

  SUCCESS("get dns address: %s", dns_string);
}

void open_tun() {
  char tun_name[] = "tun4o6";
  tunfd = open("/dev/net/tun", O_RDWR);
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
  sprint(buffer, "ip link set dev %s mtu 1500", tun_name);
  system(buffer);
}

int main() {
  get_dns();
  open_tun();
  init_user_info_list();
  main_thread();
  return 0;
}