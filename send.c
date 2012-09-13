/**
 * 发送处理
 */
#include "global.h"
#include <sys/time.h>
#include "send.h"
#include "icmp.h"
#include "result.h"

static int m_sock = -1;
static pthread_t m_thread;


static void *send_loop(void *args)
{
	int e;
	register int i;
	struct table *results = (struct table *)args;

	for (i = 0; i < g_count; i++)
	{
		table_entry_t *te;

		table_for_each(te, results)
		{
			const char *ips = te->k;
			struct ping_results *pr = (struct ping_results *)te->v;

			if (g_send_delay)
				usleep(g_send_delay);

			e = icmp_send(m_sock, &pr->addr, i);	
			if (e < 0)
			{
				log_error("icmp_send to %s error: %d", ips, e);
			}
			else
			{
				gettimeofday(&(pr->results[i].tv_sent), NULL);
				log_verbose("icmp_send to %s ok", ips);
			}
		}
	}

	log_debug("Send done");
	return NULL;
}


int send_init(struct table *results)
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

    e = pthread_create(&m_thread, NULL, send_loop, results);
    if (e < 0)
    {
        log_error("pthread_create error: %d", e);
        return -3;
    }

    return 0;
}


void send_wait(void)
{
    pthread_join(m_thread, NULL);
	log_debug("send thread finished");
    icmp_close(m_sock);
}

