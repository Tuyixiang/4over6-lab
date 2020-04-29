#include "user_info.h"

#include "main.h"
#include "msg.h"

struct UserInfo *user_info_list;
int user_info_list_count;

void init_user_info_and_lock(struct UserInfo *info) {
  pthread_mutex_init(&info->lock, NULL);
  pthread_mutex_lock(&info->lock);
  info->valid = 1;
  info->sock_in = -1;
  info->last_heartbeat = info->last_request = time(NULL);
  // info->pending_read_size = info->pending_write_size = info->pending_out_size
  // =
  //     0;
  // info->pending_read = info->pending_write = info->pending_out = NULL;
}

void free_user_info(struct UserInfo *user_info) {
  pthread_mutex_lock(&user_info->lock);
  user_info->valid = 0;
  user_info_list_count--;
  close(user_info->sock_in);
  pthread_mutex_unlock(&user_info->lock);
  pthread_mutex_destroy(&user_info->lock);
}

// 在 user_info_list 上创建 MAX_CLIENT 条的共享内存空间
void init_user_info_list() {
  // create shared memory
  int fd = shm_open(SHARED_MEMORY, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    ERR("failed to create shared memory");
  }
  user_info_list = mmap(NULL, MAX_CLIENT * sizeof(struct UserInfo),
                        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  // mark all as empty
  for (int i = 0; i < MAX_CLIENT; i++) {
    user_info_list[i].valid = 0;
    user_info_list[i].address_4.s_addr = BASE_IP + i;
  }
  assert(RECV_BUFFER_LENGTH > sizeof(struct Msg));

  user_info_list_count = 0;
}

struct UserInfo *get_locked_user_info_slot(struct sockaddr_in6 *addr) {
  int empty = -1;
  for (int i = 0; i < MAX_CLIENT; i++) {
    if (user_info_list[i].valid) {
      if (!memcmp(&user_info_list[i].address_6, addr,
                  sizeof(struct sockaddr_in6))) {
        pthread_mutex_lock(&user_info_list[i].lock);
        return &user_info_list[i];
      }
    } else {
      empty = i;
    }
  }
  if (empty >= 0) {
    struct UserInfo *slot = &user_info_list[empty];
    init_user_info_and_lock(slot);
    slot->address_6 = *addr;
    user_info_list_count++;
    return slot;
  } else {
    return NULL;
  }
}

int user_info_list_full() { return user_info_list_count >= MAX_CLIENT; }

void debug_print(const struct UserInfo *user_info) {
  LOG("user_info:");
  LOG("\tvalid:\t\t%d", user_info->valid);
  LOG("\tsock_in:\t%d", user_info->sock_in);
  LOG("\tlast_heartbeat:\t%ld", user_info->last_heartbeat);
  LOG("\tlast_request:\t%ld", user_info->last_request);
  LOG("\taddress_4:\t" IP4_FMT, IP4(user_info->address_4));
  LOG("\taddress_6:\t" IP6_FMT, IP6(user_info->address_6));
}
/*
int try_read(struct UserInfo *info) {
  pthread_mutex_lock(&info->lock);

  int result = 0;

  // try to read
  char buffer[RECV_BUFFER_LENGTH];
  int len = recv(info->sock_in, info->pending_read + info->pending_read_size,
                 RECV_BUFFER_LENGTH - info->pending_read_size, MSG_DONTWAIT);
  if (len < 0) {
    if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
      result = -1;
    }
  } else {
    LOG("%d bytes read from " IP6_FMT, len, IP6(info->address_6));
    info->pending_read_size += len;
    if (info->pending_read_size >= 5) {
      int msg_size = ((struct Msg *)info->pending_read)->length;
      if (info->pending_read_size >= msg_size) {
        // ready
        result = msg_size;
      }
    }
  }

  pthread_mutex_unlock(&info->lock);

  return result;
}

int try_write(struct UserInfo *info) {
  pthread_mutex_lock(&info->lock);
  int result = 0;

  if (info->pending_write_size > 0) {
    int len = send(info->sock_in, info->pending_write, info->pending_write_size,
                   MSG_DONTWAIT);
    if (len < 0) {
      if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
        result = -1;
      }
    } else {
      LOG("%d bytes sent to " IP6_FMT, len, IP6(info->address_6));
      if (len >= info->pending_write_size) {
        free(info->pending_write);
        info->pending_write_size = 0;
        result = 1;
      } else {
        info->pending_write_size -= len;
        char *buff = malloc(info->pending_write_size);
        memcpy(buff, info->pending_write + len, info->pending_write_size);
        free(info->pending_write);
        info->pending_write = buff;
      }
    }
  } else {
    result = 1;
  }

  pthread_mutex_unlock(&info->lock);
  return result;
}

int try_out(struct UserInfo *info) {
  pthread_mutex_lock(&info->lock);
  int result = 0;

  if (info->pending_out_size > 0) {
    int len = send(tunfd, info->pending_out, info->pending_out_size,
MSG_DONTWAIT); if (len < 0) { if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
        result = -1;
      }
    } else {
      LOG("%d bytes sent to tun", len);
      if (len >= info->pending_out_size) {
        free(info->pending_out);
        info->pending_out_size = 0;
        result = 1;
      } else {
        info->pending_out_size -= len;
        char *buff = malloc(info->pending_out_size);
        memcpy(buff, info->pending_out + len, info->pending_out_size);
        free(info->pending_out);
        info->pending_out = buff;
      }
    }
  }

  pthread_mutex_unlock(&info->lock);
  return result;
}*/