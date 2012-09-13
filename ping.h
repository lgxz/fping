#include <netinet/in.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <string>
#include <cstring>
#include <netdb.h>

class Ping
{
    public:
        Ping(std::string host) : _host(host) {}
        ~Ping() {}

        void start()
        {
            int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
            if(sock < 0)
            {
                printf("Failed to create socket!\n");
                close(sock);
                exit(1);
            }

            setuid(getuid());

            sockaddr_in pingaddr;
            memset(&pingaddr, 0, sizeof(sockaddr_in));
            pingaddr.sin_family = AF_INET;

            hostent *h = gethostbyname(_host.c_str());
            if(not h)
            {
                printf("Failed to get host by name!\n");
                close(sock);
                exit(1);
            }

            memcpy(&pingaddr.sin_addr, h->h_addr, sizeof(pingaddr.sin_addr));

            // Set the ID of the sender (will go into the ID of the echo msg)
            int pid = getpid();

            // Only want to receive the following messages
            icmp_filter filter;
            filter.data = ~((1<<ICMP_SOURCE_QUENCH) |
                            (1<<ICMP_DEST_UNREACH) |
                            (1<<ICMP_TIME_EXCEEDED) |
                            (1<<ICMP_REDIRECT) |
                            (1<<ICMP_ECHOREPLY));
            if(setsockopt(sock, SOL_RAW, ICMP_FILTER, (char *)&filter, sizeof(filter)) < 0)
            {
                perror("setsockopt(ICMP_FILTER)");
                exit(3);
            }

            // Number of valid echo receptions
            int nrec = 0;

            // Send the packet
            for(int i = 0; i < 5; ++i)
            {
                char packet[sizeof(icmphdr)];
                memset(packet, 0, sizeof(packet));

                icmphdr *pkt = (icmphdr *)packet;
                pkt->type = ICMP_ECHO;
                pkt->code = 0;
                pkt->checksum = 0;
                pkt->un.echo.id = htons(pid & 0xFFFF);
                pkt->un.echo.sequence = i;
                pkt->checksum = checksum((uint16_t *)pkt, sizeof(packet));

                int bytes = sendto(sock, packet, sizeof(packet), 0, (sockaddr *)&pingaddr, sizeof(sockaddr_in));
                if(bytes < 0)
                {
                    printf("Failed to send to receiver\n");
                    close(sock);
                    exit(1);
                }
                else if(bytes != sizeof(packet))
                {
                    printf("Failed to write the whole packet --- bytes: %d, sizeof(packet): %d\n", bytes, sizeof(packet));
                    close(sock);
                    exit(1);
                }

                while(1)
                {
                    char inbuf[192];
                    memset(inbuf, 0, sizeof(inbuf));

                    int addrlen = sizeof(sockaddr_in);
                    bytes = recvfrom(sock, inbuf, sizeof(inbuf), 0, (sockaddr *)&pingaddr, (socklen_t *)&addrlen);
                    if(bytes < 0)
                    {
                        printf("Error on recvfrom\n");
                        exit(1);
                    }
                    else
                    {
                        if(bytes < sizeof(iphdr) + sizeof(icmphdr))
                        {
                            printf("Incorrect read bytes!\n");
                            continue;
                        }

                        iphdr *iph = (iphdr *)inbuf;
                        int hlen = (iph->ihl << 2);
                        bytes -= hlen;

                        pkt = (icmphdr *)(inbuf + hlen);
                        int id = ntohs(pkt->un.echo.id);
                        if(pkt->type == ICMP_ECHOREPLY)
                        {
                            printf("    ICMP_ECHOREPLY\n");
                            if(id == pid)
                            {
                                nrec++;
                                if(i < 5) break;
                            }
                        }
                        else if(pkt->type == ICMP_DEST_UNREACH)
                        {
                            printf("    ICMP_DEST_UNREACH\n");
                            // Extract the original data out of the received message
                            int offset = sizeof(iphdr) + sizeof(icmphdr) + sizeof(iphdr);
                            if(((bytes + hlen) - offset) == sizeof(icmphdr))
                            {
                                icmphdr *p = reinterpret_cast<icmphdr *>(inbuf + offset);
                                id = ntohs(p->un.echo.id);
                                if(id == pid)
                                {
                                    printf("        IDs match!\n");
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            printf("nrec: %d\n", nrec);
        }

    private:
        int32_t checksum(uint16_t *buf, int32_t len)
        {
            int32_t nleft = len;
            int32_t sum = 0;
            uint16_t *w = buf;
            uint16_t answer = 0;

            while(nleft > 1)
            {
                sum += *w++;
                nleft -= 2;
            }

            if(nleft == 1)
            {
                *(uint16_t *)(&answer) = *(uint8_t *)w;
                sum += answer;
            }

            sum = (sum >> 16) + (sum & 0xFFFF);
            sum += (sum >> 16);
            answer = ~sum;

            return answer;
        }

        std::string _host;
};
