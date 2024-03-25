#ifndef __KVSTORE_H__
#define __KVSTORE_H__

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define BUFFER_LENGTH		512

// RCALLBACK被定义为接受一个int参数并返回一个int值的函数指针类型
typedef int (*RCALLBACK)(int fd);

struct conn_item{
	int fd;
	char rbuffer[BUFFER_LENGTH];
	int rlen;

	char wbuffer[BUFFER_LENGTH];
	int wlen;

	union{
		RCALLBACK accept_callback;
		RCALLBACK recv_callback;
	}recv_t;
	RCALLBACK send_callback;
};

void *kvstore_malloc(size_t size);
void kvstore_free(void *ptr);
void init_kvengine(void);
void epoll_entry(void);
void exit_kvengine(void);
void kvstore_request(struct conn_item *item);
int kvstore_parser_protocol(struct conn_item *item, char **tokens, int count);

#define ENABLE_ARRAY_KVENGINE	    1
#define ENABLE_RBTREE_KVENGINE		0
#define ENABLE_HASH_KVENGINE	    0

#if ENABLE_ARRAY_KVENGINE

typedef struct array_s array_t;
extern array_t Array;

int array_create(array_t *arr);
void array_destory(array_t *arr);
int array_set(array_t *arr, char *key, char *value);
char *array_get(array_t *arr, char *key);
int array_delete(array_t *arr, char *key);
int array_modify(array_t *arr, char *key, char *value);
int array_count(array_t *arr);

#endif

#if ENABLE_RBTREE_KVENGINE

typedef struct rbtree_s retree_t;
extern retree_t Tree;

int rbtree_create(retree_t *tree);
void rbtree_destory(retree_t *tree);
int rbtree_set(retree_t *tree, char *key, char *value);
char *rbtree_get(retree_t *tree, char *key);
int rbtree_delete(retree_t *tree, char *key);
int rbtree_modify(retree_t *tree, char *key, char *value);
int rbtree_count(retree_t *tree);

#endif

#if ENABLE_HASH_KVENGINE

typedef struct hashtable_s hashtable_t;
extern hashtable_t Hash;

int hash_create(hashtable_t *hash);
void hash_destory(hashtable_t *hash);
int hash_set(hashtable_t *hash, char *key, char *value);
char *hash_get(hashtable_t *hash, char *key);
int hash_delete(hashtable_t *hash, char *key);
int hash_modify(hashtable_t *hash, char *key, char *value);
int hash_count(hashtable_t *hash);

#endif

#endif
