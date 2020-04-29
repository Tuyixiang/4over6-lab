#include "user_info.h"

struct UserInfo *user_info_list;
int user_info_list_count;

void init_user_info_and_lock(struct UserInfo *user_info) {
  pthread_mutex_init(&user_info->lock, NULL);
  pthread_mutex_lock(&user_info->lock);
  user_info->valid = 1;
  user_info->last_heartbeat = user_info->last_request = time(NULL);
  user_info->pending_read_size = user_info->pending_write_size = 0;
  user_info->pending_read = user_info->pending_write = NULL;
}

void free_user_info(struct UserInfo *user_info) {
  pthread_mutex_destroy(&user_info->lock);
  user_info->valid = 0;
  user_info_list_count--;
}

// 在 user_info_list 上创建 MAX_CLIENT 条的共享内存空间
void init_user_info_list() {
  // create shared memory
  user_info_list = mmap(NULL, MAX_CLIENT * sizeof(struct UserInfo),
                        PROT_READ | PROT_WRITE, MAP_SHARED, -1, 0);
  // mark all as empty
  for (int i = 0; i < MAX_CLIENT; i++) {
    user_info_list[i].valid = 0;
  }
  user_info_list_count = 0;
}

struct UserInfo *get_locked_user_info_slot() {
  assert(!user_info_list_full());
  for (int i = 0; i < MAX_CLIENT; i++) {
    if (!user_info_list[i].valid) {
      struct UserInfo *slot = &user_info_list[i];
      init_user_info_and_lock(slot);
      user_info_list_count++;
      return slot;
    }
  }
  ERR("wtf");
}

int user_info_list_full() { return user_info_list_count >= MAX_CLIENT; }

void debug_print(const struct UserInfo *user_info) {
    LOG("user_info:");
    LOG("\tvalid:\t%d", user_info->valid);
    LOG("\tsock_in:\t%d", user_info->sock_in);
    LOG("\tsock_out:\t%d", user_info->sock_out);
    LOG("\tlast_heartbeat:\t%ld", user_info->last_heartbeat);
    LOG("\tlast_request:\t%ld", user_info->last_request);
    LOG("\taddress_4:\t" IP4_FMT, IP4(user_info->address_4.s_addr));
    LOG("\taddress_6:\t" IP6_FMT, IP6(user_info->address_6));
    LOG("\tpending_read_size:\t%d", user_info->pending_read_size);
    LOG("\tpending_write_size:\t%d", user_info->pending_write_size);
}