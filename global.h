/**
 * 全局定义
 */
#ifndef __CBB_GLOBAL_H__
#define __CBB_GLOBAL_H__

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <netdb.h>

#define NO_NED_NAMESPACE
#include "nedmalloc.h"

#include "utils.h"
#include "log.h"
#include "table.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"
#define UNUSED(x) (void)(x)

extern int g_count;
extern int g_wait;
extern int g_send_delay;

#endif

