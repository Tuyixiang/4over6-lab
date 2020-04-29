#ifndef _USER_INFO_H_
#define _USER_INFO_H_

#include "common.h"

extern struct UserInfo *user_info_list;

struct UserInfo {
  int valid;
  int sock_in;
  time_t last_heartbeat;
  time_t last_request;
  struct in_addr address_4;
  struct in6_addr address_6;
  pthread_mutex_t lock;
  int pending_read_size;
  int pending_write_size;
  char *pending_read;
  char *pending_write;
  int pending_out_size;
  char *pending_out;
};

void init_user_info_list();

void init_user_info_and_lock(struct UserInfo *);

void free_user_info(struct UserInfo *);

struct UserInfo *get_locked_user_info_slot();

int user_info_list_full();

void debug_print(const struct UserInfo *);

// >0: ready size. 0: not ready. -1: fail
int try_read(struct UserInfo *);

int try_write(struct UserInfo *);

int try_out(struct UserInfo *);

#endif  // _USER_INFO_H_