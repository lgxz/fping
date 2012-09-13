#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "table.h"


static int m_rwlock_attr_initialized = 0;
static pthread_rwlockattr_t m_rwlock_attr;

/**
 * 初始化
 */
void table_init(table_t *t)
{

	if (t != NULL)
	{
		if ( m_rwlock_attr_initialized == 0 )
		{
			m_rwlock_attr_initialized = 1;
			pthread_rwlockattr_init(&m_rwlock_attr);
			pthread_rwlockattr_setkind_np(&m_rwlock_attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
		}

		t->root = KS_RB_ROOT;
		pthread_rwlock_init(&t->lock, &m_rwlock_attr);
	}
}


/**
 * 查找元素
 */
table_entry_t *table_get(table_t *t, const char *k)
{
	table_entry_t *ret = NULL;
	ks_rb_node_t *node;

	//table_rdlock(t);
	node = t->root.rb_node;
	while (node)
	{
		int r;
		table_entry_t *te = ks_rb_entry(node, table_entry_t, node);

		r = strcmp(k, te->k);
		if (r > 0)
			node = node->rb_left;
		else if (r < 0)
			node = node->rb_right;
		else
		{
			ret = te;
			break;
		}
	}

	//table_unlock(t);
	return ret;
}


/**
 * 增加元素
 *
 * @ret true  成功增加
 * @ret false 元素已存在
 */
int table_add(table_t *t, table_entry_t *te)
{
	int e = true;
	table_entry_t *this;
	ks_rb_node_t **p, *parent = NULL;

	//table_wrlock(t);
	p = &(t->root.rb_node);
	while (*p)
	{
		int r;

		parent = *p;
		this = ks_rb_entry(parent, table_entry_t, node);

		r = strcmp(te->k, this->k);
		if (r > 0)
			p = &(*p)->rb_left;
		else if (r < 0)
			p = &(*p)->rb_right;
		else
		{
			e = false;
			goto quit;
		}
	}

	ks_rb_link_node(&te->node, parent, p);
	ks_rb_insert_color(&te->node, &t->root);

quit:
	//table_unlock(t);
	return e;
}


/**
 * 删除元素
 *
 * NOTE: 此函数不加锁，需要调用者保证
 */
void table_del(table_t *t, table_entry_t *te)
{
	//table_wrlock(t);
	ks_rb_erase(&te->node, &t->root);
	//table_unlock(t);
}

