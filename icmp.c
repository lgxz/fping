/**
 * ICMP Functions
 */
#include "global.h"

/**
 * Open an ICMP socket
 */
int icmp_open(void)
{
	int e, sock;
	struct icmp_filter filter;

	sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock < 0)
	{
		log_error("socket error: %d", sock);
		return -1;
	}

	filter.data = ~((1<<ICMP_SOURCE_QUENCH) | (1<<ICMP_DEST_UNREACH) | (1<<ICMP_TIME_EXCEEDED) | (1<<ICMP_REDIRECT) | (1<<ICMP_ECHOREPLY));
	e = setsockopt(sock, SOL_RAW, ICMP_FILTER, (char *)&filter, sizeof(filter));
	if(e < 0)
	{
		log_error("setsockopt error: %d", e);
		close(sock);
		return -2;
	}

	return sock;
}


void icmp_close(int s)
{
	close(s);
}


/**
 * Send ICMP ECHO Request
 */
int icmp_send(int sock, struct sockaddr_in *host, uint16_t seq)
{
	int bytes;
	char packet[sizeof(struct icmphdr)];
	struct icmphdr *pkt = (struct icmphdr *)packet;

	memset(packet, 0, sizeof(packet));

	pkt->type = ICMP_ECHO;
	pkt->code = 0;
	pkt->checksum = 0;
	pkt->un.echo.id = htons(getpid() & 0xFFFF);
	pkt->un.echo.sequence = htons(seq);
	pkt->checksum = checksum((uint16_t *)pkt, sizeof(packet));

	bytes = sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)host, sizeof(*host));
	if(bytes < 0 || bytes != sizeof(packet))
	{
		log_error("sendto error: %s", strerror(errno));
		return -1;
	}

	return 0;
}


/**
 * Recv ICMP packge
 */
int icmp_recv(int sock, struct sockaddr_in *host, uint16_t *seq)
{
	int bytes, hlen;
	int addrlen = sizeof(struct sockaddr_in);
	static uint8_t buf[512];
	struct iphdr *iph;
	struct icmphdr *pkt;

	bytes = recvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr *)host, (socklen_t *)&addrlen);
	if (bytes < 0)
	{
		log_error("recvfrom error: %d", bytes);
		return -1;
	}

	if (bytes < sizeof(struct iphdr) + sizeof(struct icmphdr))
	{
		log_error("Incorrect packet");
		return -2;
	}

	iph = (struct iphdr *)buf;
	hlen = (iph->ihl << 2);
	bytes -= hlen;

	pkt = (struct icmphdr *)(buf + hlen);
	//hexdump((uint8_t*)pkt, 0, bytes);
	if(pkt->type == ICMP_ECHOREPLY && ntohs(pkt->un.echo.id) == getpid())
	{
		*seq = ntohs(pkt->un.echo.sequence);
		return 0;
	}

	return -3;
}

