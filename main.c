/**
 * 主程序入口
 */
#include "global.h"
#include <getopt.h>
#include "result.h"
#include "send.h"
#include "recv.h"

#define DEF_WAIT	5
#define DEF_SEND_DELAY	3

int g_count;
int g_wait;
int g_send_delay;

static const char *m_file_in = NULL;


static void usage(const char *self)
{
    printf("Usage: %s [-c count] [-w wait_time] [-s send_delay] -f ip_file\n\n", self);
	printf("  -c	ICMP Request per IP. Default is %d\n", DEF_COUNT);
	printf("  -w	Time to wait in second after finishing send. Default is %d\n", DEF_WAIT);
	printf("  -s	Time to wait in microsecond after a request. Default is %d\n", DEF_SEND_DELAY);
	printf("  -f	IP-list file. Default is stdin\n");
}


static int args_init(int argc, char **argv)
{
	int e;
    struct option options[] = {
        {"count", 1, NULL, 'c'},
        {"wait", 1, NULL, 'w'},
        {"file", 1, NULL, 'f'},
        {"debug", 1, NULL, 'd'},
        {"delay", 1, NULL, 's'},
        {"help", 0, NULL, '?'},
        {NULL,   0, 0, 0},
    };

	g_count = DEF_COUNT;
	g_wait = DEF_WAIT;
	g_send_delay = DEF_SEND_DELAY;

    while ( (e = getopt_long(argc, argv, "s:d:c:w:f:?", options, NULL)) != EOF )
    {
        switch (e)
        {
            case 'c':
                g_count = atoi(optarg);
				if (g_count <= 0)
				{
					log_error("Invalid count: %s", optarg);
					return -1;
				}
                break;
            case 'w':
                g_wait = atoi(optarg);
				if (g_wait < 0)
				{
					log_error("Invalid wait_time: %s", optarg);
					return -2;
				}
                break;
			case 'f':
				m_file_in = optarg;
				break;
			case 'd':
				log_set_level( atoi(optarg) );
				break;
			case 's':
				g_send_delay = atoi(optarg);
				if (g_send_delay < 0)
				{
					log_error("Invalid send delay: %s", optarg);
					return -3;
				}
				break;
            case '?':
                usage(argv[0]);
				exit(0);
            default:
                log_error("Unknown opt: %c", e);
                return -4;
        }
    }

    if (g_count <= 0 || g_count > MAX_COUNT)
    {
        log_warn("Invalid count: %d, use default: %d", g_count, DEF_COUNT);
		g_count = DEF_COUNT;
    }

	log_debug("file: %s, count: %d, wait: %d, delay: %d", m_file_in, g_count, g_wait, g_send_delay);
	return 0;
}


int main(int argc, char **argv)
{
	int e; 
	FILE *fin = stdin, *fout = stdout;
	struct table *results;
	time_t start;

	if ( geteuid() != 0 )
	{
		fprintf(stderr, "Gosh! You are NOT root\n");
		return -1;
	}

	start = time(NULL);

	log_init(NULL);

	e = args_init(argc, argv);
	if (e < 0)
	{
		log_error("args_init error: %d", e);
		return -2;
	}


	if (m_file_in)
	{
		if ( (fin = fopen(m_file_in, "r")) == NULL )
		{
			log_error("open file %s error: %s", m_file_in, strerror(errno));
			return -3;
		}
	}

	e = result_init();
	if (e < 0)
	{
		log_error("result_init error: %d", e);
		return -3;
	}

	e = result_new(&results, fin);
	if (e < 0)
	{
		log_error("result_new for file %s error: %d", m_file_in, e);
		e = -4;
		goto quit;
	}

	e = recv_init(results);
	if (e < 0)
	{
		log_error("recv_init error: %d", e);
		e = -5;
		goto quit;
	}

	e = send_init(results);
	if (e < 0)
	{
		log_error("send_init error: %d", e);
		e = -6;
		goto quit;
	}

	send_wait();
	recv_wait();

	result_output(results, fout);

quit:
	result_free();
	log_debug("Total time: %u seconds", time(NULL) - start);
	return e;
}

