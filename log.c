/**
 * Simple Log
 */
#include "global.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


static int m_debug = LL_ERROR;
const char *m_levels[] = { "F", "E", "W", "I", "D", "V", NULL };


static int sindex(const char **array, const char *s)
{
    register int i;

    for (i = 0; array[i]; i++)
    {
        if (strcasecmp(array[i], s) == 0)
        {
            return i;
        }
    }

    return -1;
}


int log_get_level(void)
{
	return m_debug;
}


void log_set_level(int level)
{
	m_debug = level;
}


/**
 * 初始化
 */
void log_init(const char *name)
{
	char *value;

	if (name == NULL || *name == '\0')
	{
		name = LOG_NAME;
	}

	value = getenv(name);
	if (value != NULL && *value != '\0')
	{
		m_debug = sindex(m_levels, value);
		if (m_debug < 0)
			m_debug = atoi(value);
	}

	log_debug("debug_level = %d", m_debug);
}


/**
 * Log
 */
void log_any(int level, const char *fmt, ...)
{
    va_list ap;
    char buf[4096];
	const char *s;

	if (level > m_debug)
	{
		return;
	}

	s = (level >= 0 && level < LL_MAX) ? m_levels[level] : "?";

    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    fprintf(stderr, "%s: %s\n", s, buf);
}



