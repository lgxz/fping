/**
 * ICMP functions
 */
#ifndef __ICMP_H__
#define __ICMP_H__

int icmp_open(void);
int icmp_close(int s);

int icmp_send(int sock, struct sockaddr_in *host, uint16_t seq);
int icmp_recv(int sock, struct sockaddr_in *host, uint16_t *seq);

#endif

