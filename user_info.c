#include "user_info.h"

struct UserInfo *user_info_list;
int user_info_list_count;

void init_user_info(struct UserInfo *user_info) {
  user_info->valid = 1;
  user_info->last_heartbeat = user_info->last_request = time(NULL);
  user_info->pending_read = user_info->pending_write = NULL;
  pthread_mutex_init(&user_info->lock, NULL);
}

void destroy_user_info(struct UserInfo *user_info) {
  pthread_mutex_destroy(&user_info->lock);
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

struct UserInfo *get_user_info_slot() {
  assert(!user_info_list_full());
  for (int i = 0; i < MAX_CLIENT; i++) {
    if (!user_info_list[i].valid) {
      struct UserInfo *slot = &user_info_list[i];
      init_user_info(slot);
      user_info_list_count++;
      return slot;
    }
  }
  ERR("wtf");
}

int user_info_list_full() { return user_info_list_count >= MAX_CLIENT; }