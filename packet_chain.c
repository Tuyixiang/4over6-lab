#include "packet_chain.h"
#include "common.h"

void push_packet(struct PacketChain **target, char *buffer, int size) {
  while (*target != NULL) {
    target = &(*target)->next;
  }
  *target = malloc(sizeof(struct PacketChain));
  (*target)->buffer = buffer;
  (*target)->size = size;
  (*target)->next = NULL;
}

struct PacketChain *pop_packet(struct PacketChain **target) {
  assert(*target != NULL);
  free((*target)->buffer);
  if ((*target)->next == NULL) {
    free(*target);
    *target = NULL;
  } else {
    struct PacketChain *next = (*target)->next;
    free(*target);
    *target = next;
  }
}