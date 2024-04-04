#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kvstore.h"

#define RED    1
#define BLACK  2

typedef char* KEY_TYPE;

typedef struct _rbtree_node{
	unsigned char color;
	struct _rbtree_node *right;
	struct _rbtree_node *left;
	struct _rbtree_node *parent;
	KEY_TYPE key;
	void *value;
}rbtree_node;

typedef struct rbtree_s{
	rbtree_node *root;
	rbtree_node *nil;
	int count;
}rbtree_t;

retree_t Tree;

rbtree_node *rbtree_mini(rbtree_t *T, rbtree_node *x) {
	while (x->left != T->nil) x = x->left;
	return x;
}

rbtree_node *rbtree_maxi(rbtree_t *T, rbtree_node *x) {
	while (x->right != T->nil) x = x->right;
	return x;
}


rbtree_node *rbtree_successor(rbtree_t *T, rbtree_node *x) {
	rbtree_node *y = x->parent;

	if (x->right != T->nil) return rbtree_mini(T, x->right);
	while ((y != T->nil) && (x == y->right)) {
		x = y;
		y = y->parent;
	}
	return y;
}


void rbtree_left_rotate(rbtree_t *T, rbtree_node *x){
	rbtree_node *y = x->right;
	x->right = y->left;
	if(y->left != T->nil) y->left->parent = x;
	y->parent = x->parent;
	if(x->parent == T->nil) T->root = y;
	else if (x == x->parent->left) x->parent->left = y;
	else x->parent->right = y;
	y->left = x;
	x->parent = y;
}

void rbtree_right_rotate(rbtree_t *T, rbtree_node *y){
	rbtree_node *x = y->left;
	y->left = x->right;
	if(x->right != T->nil) x->right->parent = y;;
	x->parent = y->parent;
	if (y->parent == T->nil) T->root = x;
	else if (y == y->parent->right) y->parent->right = x;
	else y->parent->left = x;
	x->right = y;
	y->parent = x;
}

void rbtree_insert_fixup(rbtree_t *T, rbtree_node *z) {
	while (z->parent->color == RED) { 
		if (z->parent == z->parent->parent->left) {
			rbtree_node *y = z->parent->parent->right;
			if (y->color == RED) {
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent; //z --> RED
			} 
			else {
				if (z == z->parent->right) {
					z = z->parent;
					rbtree_left_rotate(T, z);
				}
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rbtree_right_rotate(T, z->parent->parent);
			}
		}
		else {
			rbtree_node *y = z->parent->parent->left;
			if (y->color == RED) {
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent; 
			} 
			else {
				if (z == z->parent->left) {
					z = z->parent;
					rbtree_right_rotate(T, z);
				}
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rbtree_left_rotate(T, z->parent->parent);
			}
		}
	}
	T->root->color = BLACK;
}


void rbtree_insert(rbtree_t *T, rbtree_node *z) {
	rbtree_node *y = T->nil;
	rbtree_node *x = T->root;
	while (x != T->nil) {
		y = x;
		if (strcmp(z->key, x->key) < 0) x = x->left;
		else if (strcmp(z->key, x->key) > 0) x = x->right;
		else return ;
	}
	z->parent = y;
	if (y == T->nil) T->root = z; 
	else if (strcmp(z->key, y->key) < 0) y->left = z; 
	else y->right = z;

	z->left = T->nil;
	z->right = T->nil;
	z->color = RED;

	rbtree_insert_fixup(T, z);
}

void rbtree_delete_fixup(rbtree_t *T, rbtree_node *x) {
	while ((x != T->root) && (x->color == BLACK)) {
		if (x == x->parent->left) {
			rbtree_node *w= x->parent->right;
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				rbtree_left_rotate(T, x->parent);
				w = x->parent->right;
			}
			if ((w->left->color == BLACK) && (w->right->color == BLACK)) {
				w->color = RED;
				x = x->parent;
			} 
			else {
				if (w->right->color == BLACK) {
					w->left->color = BLACK;
					w->color = RED;
					rbtree_right_rotate(T, w);
					w = x->parent->right;
				}
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				rbtree_left_rotate(T, x->parent);
				x = T->root;
			}
		} 
		else {
			rbtree_node *w = x->parent->left;
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				rbtree_right_rotate(T, x->parent);
				w = x->parent->left;
			}
			if ((w->left->color == BLACK) && (w->right->color == BLACK)) {
				w->color = RED;
				x = x->parent;
			} 
			else {
				if (w->left->color == BLACK) {
					w->right->color = BLACK;
					w->color = RED;
					rbtree_left_rotate(T, w);
					w = x->parent->left;
				}
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				rbtree_right_rotate(T, x->parent);
				x = T->root;
			}
		}
	}
	x->color = BLACK;
}

rbtree_node *rbtree_del(rbtree_t *T, rbtree_node *z) {
	rbtree_node *y = T->nil;
	rbtree_node *x = T->nil;
	if ((z->left == T->nil) || (z->right == T->nil)) y = z;
	else y = rbtree_successor(T, z);
	if (y->left != T->nil) x = y->left;
	else if (y->right != T->nil) x = y->right;
	x->parent = y->parent;
	if (y->parent == T->nil) T->root = x;
	else if (y == y->parent->left) y->parent->left = x;
	else y->parent->right = x;

	if (y != z) {
		void *tmp = z->key;
		z->key = y->key;
		y->key = tmp;
	
		tmp = z->value;
		z->value = y->value;
		y->value = tmp;
	}
	if (y->color == BLACK) rbtree_delete_fixup(T, x);
	return y;
}

rbtree_node *rbtree_search(rbtree_t *T, KEY_TYPE key) {
	rbtree_node *node = T->root;
	while (node != T->nil) {
		if (strcmp(key, node->key) < 0) node = node->left;
		else if (strcmp(key, node->key) > 0) node = node->right;
		else return node;	
	}
	return T->nil;
}

void rbtree_traversal(rbtree_t *T, rbtree_node *node) {
	if (node != T->nil) {
		rbtree_traversal(T, node->left);
		printf("key:%s, color:%d\n", node->key, node->color);
		rbtree_traversal(T, node->right);
	}
}

int rbtree_create(retree_t *tree){
	if(!tree) return -1;
	memset(tree, 0, sizeof(rbtree_t));
	tree->nil = (rbtree_node*)malloc(sizeof(rbtree_node));
		tree->nil->key = malloc(1);
		*(tree->nil->key) = '\0';
		tree->nil->color = BLACK;
		tree->root = tree->nil;
		return 0;
}

void rbtree_destory(retree_t *tree){
	if (!tree) return ;
	if (tree->nil->key) kvstore_free(tree->nil->key);
	rbtree_node *node = tree->root;
	while (node != tree->nil) {
		node = rbtree_mini(tree, tree->root);
		if (node == tree->nil) {
			break;
		}
		node = rbtree_del(tree, node);
		if (!node) {
			kvstore_free(node->key);
			kvstore_free(node->value);
			kvstore_free(node);
		}	
	}
}

int rbtree_set(retree_t *tree, char *key, char *value){
	rbtree_node *node  = (rbtree_node*)malloc(sizeof(rbtree_node));
	if (!node) return -1;
	node->key = kvstore_malloc(strlen(key) + 1);
	if (node->key == NULL) {
		kvstore_free(node);
		return -1;
	}	
	memset(node->key, 0, strlen(key) + 1);
	strcpy(node->key, key);
	node->value = kvstore_malloc(strlen(value) + 1);
	if (node->value == NULL) {
		kvstore_free(node->key);
		kvstore_free(node);
		return -1;
	}
	memset(node->value, 0, strlen(value) + 1);
	strcpy((char *)node->value, value);
	rbtree_insert(tree, node);
	tree->count ++;
	return 0;
}

char *rbtree_get(retree_t *tree, char *key){
	rbtree_node *node = rbtree_search(tree, key);
	if (node == tree->nil) {
		return NULL;
	}
	return node->value;
}

int rbtree_delete(retree_t *tree, char *key){
	rbtree_node *node = rbtree_search(tree, key);
	if (node == tree->nil) {
		return -1;
	}
	rbtree_node *cur = rbtree_del(tree, node);
	if (!cur) {
		kvstore_free(cur->key);
		kvstore_free(cur->value);
		kvstore_free(cur);
	}
	tree->count --;
	return 0;
}

int rbtree_modify(retree_t *tree, char *key, char *value){
	rbtree_node *node = rbtree_search(tree, key);
	if (node == tree->nil) {
		return -1;
	}
	char *tmp = node->value;
	kvstore_free(tmp);
	node->value = kvstore_malloc(strlen(value) + 1);
	if (node->value == NULL) {
		return -1;
	}
	strcpy(node->value, value);
	return 0;
}

int rbtree_count(retree_t *tree){
	return tree->count;
}
