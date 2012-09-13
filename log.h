/**
 * Simple Log Interface
 */
#ifndef __CBB_LOG_H__
#define __CBB_LOG_H__

#define LOG_NAME	"DEBUG_LEVEL"

enum
{
	LL_FATAL,
	LL_ERROR,
	LL_WARN,
	LL_INFO,
	LL_DEBUG,
	LL_VERBOSE,
	LL_MAX,
};

extern void log_init(const char *name);
extern void log_set_level(int level);
extern int  log_get_level(void);
extern void log_any(int level, const char *fmt, ...);

#define log_verbose(fmt, args...)	do { log_any(LL_VERBOSE, "%s: " fmt, __FUNCTION__, ## args); } while(0)
#define log_debug(fmt, args...)		do { log_any(LL_DEBUG, "%s: " fmt, __FUNCTION__, ## args); } while(0)
#define log_info(fmt, args...)		do { log_any(LL_INFO, "%s: " fmt, __FUNCTION__, ## args); } while(0)
#define log_warn(fmt, args...)		do { log_any(LL_WARN, "%s: " fmt, __FUNCTION__, ## args); } while(0)
#define log_error(fmt, args...)		do { log_any(LL_ERROR, "%s: " fmt, __FUNCTION__, ## args); } while(0)
#define log_fatal(fmt, args...)		do { log_any(LL_FATAL, "%s: " fmt, __FUNCTION__, ## args); } while(0)

#endif
