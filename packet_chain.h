#ifndef _PACKET_CHAIN_H_
#define _PACKET_CHAIN_H_

struct PacketChain {
  char *buffer;
  struct PacketChain *next;
  int size;
};

void push_packet(struct PacketChain **, char *, int);

struct PacketChain *pop_packet(struct PacketChain **);

#endif  // _PACKET_CHAIN_H_