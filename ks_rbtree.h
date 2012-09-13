/**
 * @file
 * @brief RBtree, borrow from Linux:include/linux/rbtree.h
 */
#ifndef _KS_RBTREE_H
#define _KS_RBTREE_H

#include <stdio.h>
#include <stdint.h>

#ifndef offsetof
#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({                \
    typeof( ((type *)0)->member ) *__mptr = (ptr);        \
    (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#define KS_RB_RED      0
#define KS_RB_BLACK    1

typedef struct _ks_rb_node {
    unsigned long        rb_parent_color;
    struct _ks_rb_node  *rb_right;
    struct _ks_rb_node  *rb_left;
} ks_rb_node_t __attribute__((aligned(sizeof(long))));

typedef struct {
    ks_rb_node_t *rb_node;
} ks_rb_root_t;

#define ks_rb_parent(r)    ((ks_rb_node_t *)((r)->rb_parent_color & ~3))
#define ks_rb_color(r)     ((r)->rb_parent_color & 1)
#define ks_rb_is_red(r)    (!ks_rb_color(r))
#define ks_rb_is_black(r)  ks_rb_color(r)

static inline void ks_rb_link_node(ks_rb_node_t * node, ks_rb_node_t * parent, ks_rb_node_t ** rb_link)
{
    node->rb_parent_color = (unsigned long )parent;
    node->rb_left = node->rb_right = NULL;

    *rb_link = node;
}

#define KS_RB_ROOT                     (ks_rb_root_t) { NULL, }
#define ks_rb_entry(ptr, type, member) container_of(ptr, type, member)

#define KS_RB_EMPTY_ROOT(root)    ((root)->rb_node == NULL)
#define KS_RB_EMPTY_NODE(node)    (ks_rb_parent(node) == node)
#define KS_RB_CLEAR_NODE(node)    (ks_rb_set_parent(node, node))

#ifdef __cplusplus
extern "C" {
#endif

void ks_rb_insert_color(ks_rb_node_t *, ks_rb_root_t *);
void ks_rb_erase(ks_rb_node_t *, ks_rb_root_t *);
/* Fast replacement of a single node without remove/rebalance/add/rebalance */
void ks_rb_replace_node(ks_rb_node_t *victim, ks_rb_node_t *n, ks_rb_root_t *root);

ks_rb_node_t *ks_rb_next(ks_rb_node_t *);
ks_rb_node_t *ks_rb_prev(ks_rb_node_t *);
ks_rb_node_t *ks_rb_first(ks_rb_root_t *);
ks_rb_node_t *ks_rb_last(ks_rb_root_t *);

typedef void (*ks_rb_augment_f)(ks_rb_node_t *node, void *data);

void ks_rb_augment_insert(ks_rb_node_t *node, ks_rb_augment_f func, void *data);
void ks_rb_augment_erase_end(ks_rb_node_t *node, ks_rb_augment_f func, void *data);
ks_rb_node_t *ks_rb_augment_erase_begin(ks_rb_node_t *node);

#ifdef __cplusplus
}
#endif

#endif
