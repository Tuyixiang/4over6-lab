#ifndef _USER_INFO_H_
#define _USER_INFO_H_

#include "common.h"
#include "packet_chain.h"

extern struct UserInfo *user_info_list;

struct UserInfo {
  int valid;
  int sock_in;
  time_t last_heartbeat;
  time_t last_request;
  struct in_addr address_4;
  struct sockaddr_in6 address_6;
  pthread_mutex_t lock;
};

void init_user_info_list();

void init_user_info_and_lock(struct UserInfo *);

void free_user_info(struct UserInfo *);

struct UserInfo *get_locked_user_info_slot(struct sockaddr_in6 *);

int user_info_list_full();

void debug_print(const struct UserInfo *);

#endif  // _USER_INFO_H_