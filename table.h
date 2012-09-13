/**
 * Table 实现
 */
#ifndef __TABLE_H__
#define __TABLE_H__

#include <pthread.h>
#include "ks_rbtree.h"

typedef struct table_entry
{
	ks_rb_node_t node;
	const char *k;
	void *v;
} table_entry_t;

typedef struct table
{
	ks_rb_root_t root;
	pthread_rwlock_t lock;
} table_t;

#define TABLE_INIT	{ KS_RB_ROOT, PTHREAD_RWLOCK_INITIALIZER }

extern void table_init(table_t *t);
extern int table_add(table_t *t, table_entry_t *te);
extern void table_del(table_t *t, table_entry_t *te);
extern table_entry_t *table_get(table_t *t, const char *k);

#define table_rdlock(t)		pthread_rwlock_rdlock(&(t)->lock)
#define table_wrlock(t)		pthread_rwlock_wrlock(&(t)->lock)
#define table_unlock(t)		pthread_rwlock_unlock(&(t)->lock)

#define table_empty(t)		KS_RB_EMPTY_ROOT(&(t)->root)

#define table_first(t)		ks_rb_first(&((t)->root))

#define table_for_each(ke, t)	\
	for (ks_rb_node_t *node = table_first(t); node && (ke=ks_rb_entry(node, table_entry_t, node)); node = ks_rb_next(node))

#endif

