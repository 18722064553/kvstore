#include "kvstore.h"

void *kvstore_malloc(size_t size){
	return malloc(size);
}

void kvstore_free(void * ptr){
	return free(ptr);
}

#if ENABLE_ARRAY_KVENGINE
int kvstore_array_set(char *key, char *value) {
	return array_set(&Array, key, value);
}
char *kvstore_array_get(char *key) {
	return array_get(&Array, key);
}
int kvstore_array_delete(char *key) {
	return array_delete(&Array, key);
}
int kvstore_array_modify(char *key, char *value) {
	return array_modify(&Array, key, value);
}
int kvstore_array_count(void) {
	return array_count(&Array);
}
#endif

#if ENABLE_RBTREE_KVENGINE
int kvstore_rbtree_set(char *key, char *value) {
	return rbtree_set(&Tree, key, value);
}
char* kvstore_rbtree_get(char *key) {
	return rbtree_get(&Tree, key);
}
int kvstore_rbtree_delete(char *key) {
	return rbtree_delete(&Tree, key);
}
int kvstore_rbtree_modify(char *key, char *value) {
	return rbtree_modify(&Tree, key, value);
}
int kvstore_rbtree_count(void) {
	return rbtree_count(&Tree);
}
#endif

#if ENABLE_HASH_KVENGINE
int kvstore_hash_set(char *key, char *value) {
	return hash_set(&Hash, key, value);
}
char *kvstore_hash_get(char *key) {
	return hash_get(&Hash, key);
}
int kvstore_hash_delete(char *key) {
	return hash_delete(&Hash, key);
}
int kvstore_hash_modify(char *key, char *value) {
	return hash_modify(&Hash, key, value);
}
int kvstore_hash_count(void) {
	return kvs_hash_count(&Hash);
}
#endif



void init_kvengine(void){
#if ENABLE_ARRAY_KVENGINE
	array_create(&Array);
#endif

#if ENABLE_RBTREE_KVENGINE
	rbtree_create(&Tree);
#endif

#if ENABLE_HASH_KVENGINE
	hash_create(&Hash);
#endif
}

void exit_kvengine(void){
#if ENABLE_ARRAY_KVENGINE
	array_destory(&Array);
#endif

#if ENABLE_RBTREE_KVENGINE
	rbtree_destory(&Tree);
#endif

#if ENABLE_HASH_KVENGINE
	hash_destory(&Hash);
#endif
}

int main(){
	init_kvengine();
	epoll_entry();
	exit_kvengine();
	return 0;
}
