/**
 * @file
 * @brief Implementation of RB-tree, copy from Linux:lib/rbtree.c
 */
#include "ks_rbtree.h"

#define ks_rb_set_red(r)   do { (r)->rb_parent_color &= ~1; } while (0)
#define ks_rb_set_black(r) do { (r)->rb_parent_color |= 1; } while (0)

static inline void ks_rb_set_parent(ks_rb_node_t *rb, ks_rb_node_t *p)
{
    rb->rb_parent_color = (rb->rb_parent_color & 3) | (unsigned long)p;
}

static inline void ks_rb_set_color(ks_rb_node_t *rb, int color)
{
    rb->rb_parent_color = (rb->rb_parent_color & ~1) | color;
}

static void __rb_rotate_left(ks_rb_node_t *node, ks_rb_root_t *root)
{
    ks_rb_node_t *right = node->rb_right;
    ks_rb_node_t *parent = ks_rb_parent(node);

    if ((node->rb_right = right->rb_left))
        ks_rb_set_parent(right->rb_left, node);
    right->rb_left = node;

    ks_rb_set_parent(right, parent);

    if (parent) {
        if (node == parent->rb_left)
            parent->rb_left = right;
        else
            parent->rb_right = right;
    } else {
        root->rb_node = right;
    }
    ks_rb_set_parent(node, right);
}

static void __rb_rotate_right(ks_rb_node_t *node, ks_rb_root_t *root)
{
    ks_rb_node_t *left = node->rb_left;
    ks_rb_node_t *parent = ks_rb_parent(node);

    if ((node->rb_left = left->rb_right))
        ks_rb_set_parent(left->rb_right, node);
    left->rb_right = node;

    ks_rb_set_parent(left, parent);

    if (parent) {
        if (node == parent->rb_right)
            parent->rb_right = left;
        else
            parent->rb_left = left;
    } else {
        root->rb_node = left;
    }
    ks_rb_set_parent(node, left);
}

void ks_rb_insert_color(ks_rb_node_t *node, ks_rb_root_t *root)
{
    ks_rb_node_t *parent, *gparent;

    while ((parent = ks_rb_parent(node)) && ks_rb_is_red(parent)) {
        gparent = ks_rb_parent(parent);

        if (parent == gparent->rb_left) {
            register ks_rb_node_t *uncle = gparent->rb_right;
            if (uncle && ks_rb_is_red(uncle)) {
                ks_rb_set_black(uncle);
                ks_rb_set_black(parent);
                ks_rb_set_red(gparent);
                node = gparent;
                continue;
            }

            if (parent->rb_right == node) {
                register ks_rb_node_t *tmp;
                __rb_rotate_left(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            ks_rb_set_black(parent);
            ks_rb_set_red(gparent);
            __rb_rotate_right(gparent, root);
        } else {
            register ks_rb_node_t *uncle = gparent->rb_left;
            if (uncle && ks_rb_is_red(uncle)) {
                ks_rb_set_black(uncle);
                ks_rb_set_black(parent);
                ks_rb_set_red(gparent);
                node = gparent;
                continue;
            }

            if (parent->rb_left == node) {
                register ks_rb_node_t *tmp;
                __rb_rotate_right(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            ks_rb_set_black(parent);
            ks_rb_set_red(gparent);
            __rb_rotate_left(gparent, root);
        }
    }

    ks_rb_set_black(root->rb_node);
}

static void __rb_erase_color(ks_rb_node_t *node, ks_rb_node_t *parent, ks_rb_root_t *root)
{
    ks_rb_node_t *other;

    while ((!node || ks_rb_is_black(node)) && node != root->rb_node) {
        if (parent->rb_left == node) {
            other = parent->rb_right;
            if (ks_rb_is_red(other)) {
                ks_rb_set_black(other);
                ks_rb_set_red(parent);
                __rb_rotate_left(parent, root);
                other = parent->rb_right;
            }
            if ((!other->rb_left || ks_rb_is_black(other->rb_left)) &&
                (!other->rb_right || ks_rb_is_black(other->rb_right))) {
                ks_rb_set_red(other);
                node = parent;
                parent = ks_rb_parent(node);
            } else {
                if (!other->rb_right || ks_rb_is_black(other->rb_right)) {
                    ks_rb_set_black(other->rb_left);
                    ks_rb_set_red(other);
                    __rb_rotate_right(other, root);
                    other = parent->rb_right;
                }
                ks_rb_set_color(other, ks_rb_color(parent));
                ks_rb_set_black(parent);
                ks_rb_set_black(other->rb_right);
                __rb_rotate_left(parent, root);
                node = root->rb_node;
                break;
            }
        } else {
            other = parent->rb_left;
            if (ks_rb_is_red(other)) {
                ks_rb_set_black(other);
                ks_rb_set_red(parent);
                __rb_rotate_right(parent, root);
                other = parent->rb_left;
            }
            if ((!other->rb_left || ks_rb_is_black(other->rb_left)) &&
                (!other->rb_right || ks_rb_is_black(other->rb_right))) {
                ks_rb_set_red(other);
                node = parent;
                parent = ks_rb_parent(node);
            } else {
                if (!other->rb_left || ks_rb_is_black(other->rb_left)) {
                    ks_rb_set_black(other->rb_right);
                    ks_rb_set_red(other);
                    __rb_rotate_left(other, root);
                    other = parent->rb_left;
                }
                ks_rb_set_color(other, ks_rb_color(parent));
                ks_rb_set_black(parent);
                ks_rb_set_black(other->rb_left);
                __rb_rotate_right(parent, root);
                node = root->rb_node;
                break;
            }
        }
    }
    if (node)
        ks_rb_set_black(node);
}

void ks_rb_erase(ks_rb_node_t *node, ks_rb_root_t *root)
{
    ks_rb_node_t *child, *parent;
    int color;

    if (!node->rb_left) {
        child = node->rb_right;
    } else if (!node->rb_right) {
        child = node->rb_left;
    } else {
        ks_rb_node_t *old = node, *left;

        node = node->rb_right;
        while ((left = node->rb_left) != NULL)
            node = left;

        if (ks_rb_parent(old)) {
            if (ks_rb_parent(old)->rb_left == old)
                ks_rb_parent(old)->rb_left = node;
            else
                ks_rb_parent(old)->rb_right = node;
        } else
            root->rb_node = node;

        child = node->rb_right;
        parent = ks_rb_parent(node);
        color = ks_rb_color(node);

        if (parent == old) {
            parent = node;
        } else {
            if (child)
                ks_rb_set_parent(child, parent);
            parent->rb_left = child;

            node->rb_right = old->rb_right;
            ks_rb_set_parent(old->rb_right, node);
        }

        node->rb_parent_color = old->rb_parent_color;
        node->rb_left = old->rb_left;
        ks_rb_set_parent(old->rb_left, node);

        goto color;
    }

    parent = ks_rb_parent(node);
    color = ks_rb_color(node);

    if (child) ks_rb_set_parent(child, parent);
    if (parent) {
        if (parent->rb_left == node)
            parent->rb_left = child;
        else
            parent->rb_right = child;
    } else {
        root->rb_node = child;
    }

 color:
    if (color == KS_RB_BLACK)
        __rb_erase_color(child, parent, root);
}

static void rb_augment_path(ks_rb_node_t *node, ks_rb_augment_f func, void *data)
{
    ks_rb_node_t *parent;

up:
    func(node, data);
    parent = ks_rb_parent(node);
    if (!parent) return;

    if (node == parent->rb_left && parent->rb_right) {
        func(parent->rb_right, data);
    } else if (parent->rb_left) {
        func(parent->rb_left, data);
    }

    node = parent;
    goto up;
}

/*
 * after inserting @node into the tree, update the tree to account for
 * both the new entry and any damage done by rebalance
 */
void ks_rb_augment_insert(ks_rb_node_t *node, ks_rb_augment_f func, void *data)
{
    if (node->rb_left)
        node = node->rb_left;
    else if (node->rb_right)
        node = node->rb_right;

    rb_augment_path(node, func, data);
}

/*
 * before removing the node, find the deepest node on the rebalance path
 * that will still be there after @node gets removed
 */
ks_rb_node_t *ks_rb_augment_erase_begin(ks_rb_node_t *node)
{
    ks_rb_node_t *deepest;

    if (!node->rb_right && !node->rb_left)
        deepest = ks_rb_parent(node);
    else if (!node->rb_right)
        deepest = node->rb_left;
    else if (!node->rb_left)
        deepest = node->rb_right;
    else {
        deepest = ks_rb_next(node);
        if (deepest->rb_right)
            deepest = deepest->rb_right;
        else if (ks_rb_parent(deepest) != node)
            deepest = ks_rb_parent(deepest);
    }

    return deepest;
}

/*
 * after removal, update the tree to account for the removed entry
 * and any rebalance damage.
 */
void ks_rb_augment_erase_end(ks_rb_node_t *node, ks_rb_augment_f func, void *data)
{
    if (node)
        rb_augment_path(node, func, data);
}

/*
 * This function returns the first node (in sort order) of the tree.
 */
ks_rb_node_t *ks_rb_first(ks_rb_root_t *root)
{
    ks_rb_node_t    *n;

    n = root->rb_node;
    if (!n) return NULL;
    while (n->rb_left) n = n->rb_left;
    return n;
}

ks_rb_node_t *ks_rb_last(ks_rb_root_t *root)
{
    ks_rb_node_t    *n;

    n = root->rb_node;
    if (!n) return NULL;
    while (n->rb_right) n = n->rb_right;
    return n;
}

ks_rb_node_t *ks_rb_next(ks_rb_node_t *node)
{
    ks_rb_node_t *parent;

    if (ks_rb_parent(node) == node) return NULL;

    /* If we have a right-hand child, go down and then left as far
       as we can. */
    if (node->rb_right) {
        node = node->rb_right;
        while (node->rb_left) node=node->rb_left;
        return (ks_rb_node_t *)node;
    }

    /* No right-hand children.  Everything down and left is
       smaller than us, so any 'next' node must be in the general
       direction of our parent. Go up the tree; any time the
       ancestor is a right-hand child of its parent, keep going
       up. First time it's a left-hand child of its parent, said
       parent is our 'next' node. */
    while ((parent = ks_rb_parent(node)) && node == parent->rb_right)
        node = parent;

    return parent;
}

ks_rb_node_t *ks_rb_prev(ks_rb_node_t *node)
{
    ks_rb_node_t *parent;

    if (ks_rb_parent(node) == node)
        return NULL;

    /* If we have a left-hand child, go down and then right as far
       as we can. */
    if (node->rb_left) {
        node = node->rb_left;
        while (node->rb_right) node=node->rb_right;
        return (ks_rb_node_t *)node;
    }

    /* No left-hand children. Go up till we find an ancestor which
       is a right-hand child of its parent */
    while ((parent = ks_rb_parent(node)) && node == parent->rb_left)
        node = parent;

    return parent;
}

void ks_rb_replace_node(ks_rb_node_t *victim, ks_rb_node_t *n, ks_rb_root_t *root)
{
    ks_rb_node_t *parent = ks_rb_parent(victim);

    /* Set the surrounding nodes to point to the replacement */
    if (parent) {
        if (victim == parent->rb_left)
            parent->rb_left = n;
        else
            parent->rb_right = n;
    } else {
        root->rb_node = n;
    }
    if (victim->rb_left)
        ks_rb_set_parent(victim->rb_left, n);
    if (victim->rb_right)
        ks_rb_set_parent(victim->rb_right, n);

    /* Copy the pointers/colour from the victim to the replacement */
    *n = *victim;
}
