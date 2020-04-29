#include "main.h"

#include "main_thread.h"
#include <stdio.h>

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
  tunfd = open("/dev/net/tun", O_RDWR);
  if (tunfd == -1) {
    ERR("failed to open tun");
  }
}

int main() {
  get_dns();
  open_tun();
  init_user_info_list();
  main_thread();
  return 0;
}