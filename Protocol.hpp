#pragma once
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/ip.h>


/*************

  8b   4b   20b
-----------------
| ID | ST | TTL |
-----------------
| PAYLOAD (OPT) | <- URL p/ex
-----------------

*************/

#define LOAD_URL 0x0
#define KILL 0x1
#define LIST 0x2

struct protohdr {
    unsigned char id;
    unsigned char status:4;
    unsigned int ttl:20;
    char* payload;
  };

class Protocol {
  unsigned short in_cksum(unsigned short*, int);
  int commTunnel, optval;

public:
  struct protohdr* phdr;

  Protocol();
  int sendMsg(char*, char*);
  struct protohdr* recvMsg();
  void fillPayload(char*, int);
};
