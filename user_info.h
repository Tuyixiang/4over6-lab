#ifndef _USER_INFO_H_

#include "common.h"

struct UserInfo {
  int valid;
  int sock;
  time_t last_heartbeat;
  time_t last_request;
  struct in_addr address_4;
  struct in6_addr address_6;
  pthread_mutex_t lock;
  char *pending_read;
  char *pending_write;
};

extern struct UserInfo *user_info_list;

void init_user_info_list();

void init_user_info(struct UserInfo *);

void destroy_user_info(struct UserInfo *);

struct UserInfo *get_user_info_slot();

int user_info_list_full();

#endif  // _USER_INFO_H_