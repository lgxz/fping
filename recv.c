/**
 * 接收处理
 */
#include "global.h"
#include <sys/time.h>
#include "result.h"
#include "recv.h"
#include "icmp.h"

static int m_sock = -1;
static pthread_t m_thread;


static void *recv_loop(void *args)
{
	int e;
	struct table *results = (struct table *)args;

	while ( 1 )
	{
		uint16_t seq;
		struct sockaddr_in addr;

		e = icmp_recv(m_sock, &addr, &seq);
		if (e == 0)
		{
			table_entry_t *te;
			struct timeval tv;
			char *ips;
		   
			gettimeofday(&tv, NULL);

			ips = inet_ntoa(addr.sin_addr);
			log_verbose("recv from %s, seq = %d", ips, seq);

			if (seq > g_count)
			{
				log_warn("ignore for bad seq");
				continue;
			}

			te = table_get(results, ips);
			if (te == NULL)
			{
				log_warn("ignore for unknown IP");
			}
			else
			{
				struct ping_results *pr = (struct ping_results *)te->v;

				memcpy(&(pr->results[seq].tv_recv), &tv, sizeof(tv));
			}
		}
	}

	return NULL;
}


int recv_init(struct table *results)
{
	int e;

	if (results == NULL)
	{
		log_error("Invalid args");
		return -1;
	}

	m_sock = icmp_open();
	if (m_sock < 0)
	{
		log_error("icmp_open error: %d", m_sock);
		return -2;
	}

	e = pthread_create(&m_thread, NULL, recv_loop, results);
	if (e < 0)
	{
		log_error("pthread_create error: %d", e);
		return -3;
	}

	return 0;
}


int recv_wait(void)
{
	log_debug("recv waiting ...");

	sleep(g_wait);
	pthread_cancel(m_thread);
	icmp_close(m_sock);

	return 0;
}

