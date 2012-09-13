/**
 * ping 探测结果
 */
#ifndef __FPING_RESULT_H__
#define __FPING_RESULT_H__

//每IP最大发送请求数
#define MAX_COUNT	5

//每IP默认发送请求数
#define DEF_COUNT	3

struct ping_result
{
	struct timeval tv_sent;
	struct timeval tv_recv;
};

struct ping_results
{
	struct sockaddr_in addr;
	struct ping_result results[MAX_COUNT];
};

extern int result_init(void);
extern void result_free(void);
extern void result_dump(struct table *results);
extern void result_output(struct table *results, FILE *fp);

extern int result_new(struct table **out, FILE *fp);

#endif

