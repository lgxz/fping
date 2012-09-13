/**
 * 结果数据处理
 */
#include "global.h"
#include <sys/time.h>
#include "table.h"
#include "result.h"


static nedpool *m_pool = NULL;

/**
 * 输入 IP 列表文件，构造结果对象
 */
int result_new(struct table **out, FILE *fp)
{
	int lineno;
	char buf[64], *line;
	struct table *results;

	if (out == NULL || fp == NULL)
	{
		log_error("Invalid args");
		return -1;
	}

	if (m_pool == NULL)
	{
		log_error("result NOT inited");
		return -2;
	}

	results = (struct table *)nedpmalloc(m_pool, sizeof(*results));
	if (results == NULL)
	{
		log_error("nedpmalloc failed");
		return -3;
	}
	table_init(results);

	lineno = 0;
	while ( (line = fgets(buf, sizeof(buf), fp)) != NULL )
	{
		in_addr_t ip;
		struct table_entry *te;
		struct ping_results *pr;

		lineno ++;

		line = strip(line);
		if ( *line == '\0' )
		{
			log_warn("Ignore blank line #%d", lineno);
			continue;
		}

		ip = inet_addr(line);
		if (ip == INADDR_NONE)
		{
			log_warn("Ignore invalid line #%d: %s", lineno, line);
			continue;
		}

		pr = (struct ping_results *)nedpmalloc(m_pool, sizeof(struct ping_results));
		bzero(pr, sizeof(*pr));
		pr->addr.sin_addr.s_addr = ip;

		te = (struct table_entry *)nedpmalloc(m_pool, sizeof(*te));
		te->k = nedpstrdup(m_pool, line);
		te->v = pr;
		table_add(results, te);
	}

	*out = results;
	return 0;
}


static int calc_time(struct ping_result *prs)
{
	int count, times;
	register int i;

	times = 0;
	count = 0;

	for (i = 0; i < g_count; i++)
	{
		struct timeval *tv_recv = &(prs[i].tv_recv);
		struct timeval *tv_send = &(prs[i].tv_sent);

		if (tv_recv->tv_sec && tv_send->tv_sec)
		{
			struct timeval tv;

			count++;
			timersub(tv_recv, tv_send, &tv);
			times += tv.tv_sec  * 1000000 + tv.tv_usec;
		}
	}

	if (count)
	{
		times /= count;
	}
	else
	{
		times = -1;
	}

	return times;
}


void result_dump(struct table *results)
{
	table_entry_t *te;

	if (results == NULL)
	{
		return;
	}

	table_for_each(te, results)
	{
		const char *ips = te->k;
		struct ping_results *pr = (struct ping_results *)te->v;

		fprintf(stdout, "%s: %s\n", ips, inet_ntoa(pr->addr.sin_addr));
	}
}


void result_output(struct table *results, FILE *fp)
{
	int dead, alive;
	table_entry_t *te;

	if (results == NULL)
	{
		return;
	}

	dead = alive = 0;
	table_for_each(te, results)
	{
		int t;
		const char *ips = te->k;
		struct ping_results *pr = (struct ping_results *)te->v;

		t = calc_time(pr->results);
		if (t < 0)
		{
			dead ++;
		}
		else
		{
			alive ++;
		}
		fprintf(fp, "%s %d\n", ips, t);
	}

	log_debug("num dead: %d, num alive: %d", dead, alive);
}


int result_init(void)
{
	if (m_pool != NULL)
	{
		log_error("re-init result");
		return -1;
	}

	m_pool = nedcreatepool(0, 0);
	if (m_pool == NULL)
	{
		log_error("nedcreatepool failed");
		return -2;
	}

	return 0;
}


void result_free(void)
{
	if (m_pool)
	{
		neddestroypool(m_pool);
	}
}

