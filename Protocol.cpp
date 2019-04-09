#include "Protocol.hpp"

unsigned short Protocol::in_cksum(unsigned short *addr, int len){

  register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;
    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1)
    {
      sum += *w++;
      nleft -= 2;
    }
    /* mop up an odd byte, if necessary */
    if (nleft == 1)
    {
      *(u_char *) (&answer) = *(u_char *) w;
      sum += answer;
    }
    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
    sum += (sum >> 16);             /* add carry */
    answer = ~sum;              /* truncate to 16 bits */
    return (answer);

}

Protocol::Protocol(){
  commTunnel = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

  phdr = (struct protohdr*) malloc(sizeof(struct protohdr));

  phdr->status = phdr-> id = phdr->ttl = 0;

  if(setsockopt(commTunnel, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int)) < 0){
    throw "Error setting socket options for header included. Have you started as root?";
  }
}

int Protocol::sendMsg(char* src, char* dst){
  
  char* packet = malloc(sizeof(struct iphdr) + sizeof(struct protohdr));

  struct iphdr* ip;
  ip = (struct iphdr*) (packet); // Append it to the head of the packet

  // Start writting to the packet
  ip->tos = 0b00010000;
  ip->ihl = 5;
  ip->version = 4;
  ip->tot_len = sizeof(struct iphdr) + sizeof(struct protohdr);
  ip->protocol = IPPROTO_RAW;
  ip->saddr = inet_addr(src);
  ip->daddr = inet_addr(dst);
  ip->check = in_cksum((unsigned short*)ip, sizeof(iphdr));

  struct protohdr* customhdr;
  customhdr = (struct protohdr*) (packet + sizeof(iphdr));
  customhdr->id = phdr->id;
  customhdr->status = phdr->status;
  customhdr->ttl = phdr->ttl;
  customhdr->payload = phdr->payload;

  struct sockaddr_in dAddr;

  dAddr.sin_family = AF_INET;
  dAddr.sin_addr.s_addr = ip->daddr;

  if(sendto(commTunnel, packet, ip->tot_len, 0, (struct sockaddr*)&dAddr, sizeof(dAddr)) < 0){
    return -1;
  }

  return (int)ip->tot_len;

}

struct protohdr* Protocol::recvMsg(){
  char* packet = malloc(sizeof(struct iphdr) + sizeof(struct protohdr));
  
  if(recv(commTunnel, packet, sizeof(struct iphdr) + sizeof(struct protohdr), 0) < 0){
    return NULL;
  }

  return ( (struct protohdr*) (packet + sizeof(struct iphdr)) );
}

void Protocol::fillPayload(char* str, int size){
  phdr->payload = (char*) malloc(size);
  for(int i = 0; i < size; i++)
    phdr->payload[i] = '\0';
  strncpy(phdr->payload, str, size);
}
