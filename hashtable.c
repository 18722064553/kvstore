#include <string.h>
#include <stdlib.h>

#include "kvstore.h"

#define MAX_TABLE_SIZE 1024

typedef struct hashnode_s {
	char *key;
	char *value;
	struct hashnode_s *next;
} hashnode_t;


typedef struct hashtable_s {
	hashnode_t **nodes; 
	int max_slots;
	int count;
} hashtable_t;


hashtable_t Hash;


// TODO WHY?
static int _hash(char *key, int size) {
	if (!key) return -1;
	int sum = 0;
	int i = 0;
	while (key[i] != 0) {
		sum += key[i];
		i ++;
	}
	return sum % size;
}

hashnode_t* create_hashnode(char* key, char* value){
	hashnode_t *node = (hashnode_t*)kvstore_malloc(sizeof(hashnode_t))	;
	if(!node) return 0;
	node->key = kvstore_malloc(strlen(key) + 1);
	if(!node->key){
		kvstore_free(node);
		return NULL;
	}
	strcpy(node->key, key);

	node->value = kvstore_malloc(strlen(value) + 1);
	if(!node->value){
		kvstore_free(value);
		kvstore_free(node);
		return NULL;
	}
	strcpy(node->value, value);
	node->next = NULL;
	return node;
}

int hash_create(hashtable_t *hash){
	if(!hash) return -1;
	hash->nodes = (hashnode_t**)kvstore_malloc(sizeof(hashnode_t*) * MAX_TABLE_SIZE);
	if(!hash->nodes) return -1;
	hash->max_slots = MAX_TABLE_SIZE;
	hash->count = 0;
	return 0;
}

void hash_destory(hashtable_t *hash){
	if(!hash) return;
	int i = 0;
	for(i = 0; i < hash->max_slots; ++i){
		hashnode_t* node = hash->nodes[i];
		while(!node){
			hashnode_t* tmp = node;
			node = node->next;
			hash->nodes[i] = node;
			kvstore_free(tmp);
		}
	}
	kvstore_free(hash->nodes);
}

int hash_set(hashtable_t *hash, char *key, char *value){
	if(!hash || !key || !value) return -1;
	int idx = _hash(key, MAX_TABLE_SIZE);
	hashnode_t *node = hash->nodes[idx];
	while(node){
		if(strcmp(node->key, key) == 0) return -1;
		node = node->next;
	}
	hashnode_t *new_node = create_hashnode(key, value);
	new_node->next = hash->nodes[idx];
	hash->nodes[idx] = new_node;
	hash->count++;
	return 0;
}

char *hash_get(hashtable_t *hash, char *key){
	if (!hash || !key) return NULL;
	int idx = _hash(key, MAX_TABLE_SIZE);
	hashnode_t *node = hash->nodes[idx];
	while(node){	
		if (strcmp(node->key, key) == 0) {
				return node->value;
		}
		node = node->next;
	}
	return NULL;
}

int hash_delete(hashtable_t *hash, char *key){
	if(!hash || !key) return -2;
	int idx = _hash(key, MAX_TABLE_SIZE);
	hashnode_t *head = hash->nodes[idx];
	if(head == NULL) return -1;

	// head node match
	if (strcmp(head->key, key) == 0) {
		hashnode_t *tmp = head->next;
		hash->nodes[idx] = tmp;
		if(head->key) kvstore_free(head->key);
		if(head->value) kvstore_free(head->value);
		kvstore_free(head);
		hash->count--;
		return 0;
	}

	// 链中节点match
	hashnode_t *cur = head;
	while(cur->next){
		if(strcmp(cur->next->key, key) == 0) break;
		cur = cur->next;
	}
	if(cur->next == NULL) return -1;
	hashnode_t* tmp = cur->next;
	cur->next = tmp->next;
	if(tmp->key) kvstore_free(tmp->key);
	if(tmp->value) kvstore_free(tmp->value);
	kvstore_free(tmp);
	hash->count--;
	return 0;
}

int hash_modify(hashtable_t *hash, char *key, char *value){
	if (!hash || !key || !value) return -1;
	int idx = _hash(key, MAX_TABLE_SIZE);
	hashnode_t *node = hash->nodes[idx];
	while(node){
		if (strcmp(node->key, key) == 0) {
			kvstore_free(node->value);
			node->value = kvstore_malloc(strlen(value) + 1);
			if (node->value) {
				strcpy(node->value, value);
				return 0;
			} 
			else assert(0);
		}
		node = node->next;
	}
}

int hash_count(hashtable_t *hash){
	return hash->count;
}


