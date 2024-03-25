#include "kvstore.h"

#define KVSTORE_MAX_TOKENS 128

#if ENABLE_ARRAY_KVENGINE
int kvstore_array_set(char *key, char *value);
char *kvstore_array_get(char *key);
int kvstore_array_delete(char *key);
int kvstore_array_modify(char *key, char *value);
int kvstore_array_count(void);
#endif

#if ENABLE_RBTREE_KVENGINE
int kvstore_rbtree_create(retree_t *tree);
void kvstore_rbtree_destory(retree_t *tree);
int kvstore_rbtree_set(retree_t *tree, char *key, char *value);
char *kvstore_rbtree_get(retree_t *tree, char *key);
int kvstore_rbtree_delete(retree_t *tree, char *key);
int kvstore_rbtree_modify(retree_t *tree, char *key, char *value);
int kvstore_rbtree_count(retree_t *tree);
#endif

#if ENABLE_HASH_KVENGINE
int kvstore_hash_create(hashtable_t *hash);
void kvstore_hash_destory(hashtable_t *hash);
int kvstore_hash_set(hashtable_t *hash, char *key, char *value);
char *kvstore_hash_get(hashtable_t *hash, char *key);
int kvstore_hash_delete(hashtable_t *hash, char *key);
int kvstore_hash_modify(hashtable_t *hash, char *key, char *value);
int kvstore_hash_count(hashtable_t *hash);
#endif

const char *commands[] = {
	"SET", "GET", "DEL", "MOD", "COUNT",
	"RSET", "RGET", "RDEL", "RMOD", "RCOUNT",
	"HSET", "HGET", "HDEL", "HMOD", "HCOUNT",
};

enum {
	KVS_CMD_START = 0,
	KVS_CMD_SET = KVS_CMD_START,
	KVS_CMD_GET,
	KVS_CMD_DEL,
	KVS_CMD_MOD,
	KVS_CMD_COUNT,

	KVS_CMD_RSET,
	KVS_CMD_RGET,
	KVS_CMD_RDEL,
	KVS_CMD_RMOD,
	KVS_CMD_RCOUNT,

	KVS_CMD_HSET,
	KVS_CMD_HGET,
	KVS_CMD_HDEL,
	KVS_CMD_HMOD,
	KVS_CMD_HCOUNT,
	
	KVS_CMD_SIZE,
};

// 将输入的字符串 msg 按照空格进行分词，并将分词结果存储在字符串指针数组 tokens 中
// 参数 tokens 是一个指向指针的指针
int kvstore_split_token(char *msg, char**tokens){
	if(msg == NULL || tokens == NULL) return -1;
	int idx = 0;
	char *token = strtok(msg, " ");
	while(token != NULL){
		tokens[idx++] = token;
		token = strtok(NULL, " ");
	}
	return idx;
} 

// CRUD
int kvstore_parser_protocol(struct conn_item *item, char **tokens, int count){
	if (item == NULL || tokens[0] == NULL || count == 0) return -1;
	int cmd = KVS_CMD_START;
	for (; cmd < KVS_CMD_SIZE; cmd++) {
		if (strcmp(commands[cmd], tokens[0]) == 0) {
			break;
		}
	}
	char *msg = item->wbuffer;
	char *key = tokens[1];
	char *value = tokens[2];
	memset(msg, 0, BUFFER_LENGTH);

	switch (cmd) {
		// array
		case KVS_CMD_SET: 
			int res = kvstore_array_set(key, value);
			if (!res) snprintf(msg, BUFFER_LENGTH, "SUCCESS");
            else snprintf(msg, BUFFER_LENGTH, "FAILED");
			break;
		case KVS_CMD_GET: 
			char *val = kvstore_array_get(key);
			if (val) snprintf(msg, BUFFER_LENGTH, "%s", val);
            else snprintf(msg, BUFFER_LENGTH, "NO EXIST");
			break;
		case KVS_CMD_DEL: 
			int res = kvstore_array_delete(key);
			if (res < 0) snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
            else if (res == 0) snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
            else snprintf(msg, BUFFER_LENGTH, "NO EXIST");
			break;
		case KVS_CMD_MOD: 
			int res = kvstore_array_modify(key, value);
			if (res < 0)  snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
			 
            else if (res == 0) snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
            else snprintf(msg, BUFFER_LENGTH, "NO EXIST");
			break;
		case KVS_CMD_COUNT: 
			int count = kvstore_array_count();
			if (count < 0)   snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
            else snprintf(msg, BUFFER_LENGTH, "%d", count);
			break;
		// rbtree
		case KVS_CMD_RSET: 
			int res = kvstore_rbtree_set(key, value);
			if (!res) snprintf(msg, BUFFER_LENGTH, "SUCCESS"); 
            else snprintf(msg, BUFFER_LENGTH, "FAILED");
			break;
		case KVS_CMD_RGET: 
			char *val = kvstore_rbtree_get(key);
			if (val) snprintf(msg, BUFFER_LENGTH, "%s", val);
            else snprintf(msg, BUFFER_LENGTH, "NO EXIST");
			break;
		case KVS_CMD_RDEL: 
			int res = kvstore_rbtree_delete(key);
			if (res < 0)  snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
            else if (res == 0) snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
            else snprintf(msg, BUFFER_LENGTH, "NO EXIST");
			break;
		case KVS_CMD_RMOD: 
			int res = kvstore_rbtree_modify(key, value);
			if (res < 0) snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
            else if (res == 0) snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");			 
            else snprintf(msg, BUFFER_LENGTH, "NO EXIST");
			break;
		case KVS_CMD_RCOUNT: 
			int count = kvstore_rbtree_count();
			if (count < 0) snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
            else snprintf(msg, BUFFER_LENGTH, "%d", count);
			break;
		case KVS_CMD_HSET: 
			int res = kvstore_hash_set(key, value);
			if (!res) snprintf(msg, BUFFER_LENGTH, "SUCCESS");
            else snprintf(msg, BUFFER_LENGTH, "FAILED");
			break;
		// hash
		case KVS_CMD_HGET: 
			char *val = kvstore_hash_get(key);
			if (val) snprintf(msg, BUFFER_LENGTH, "%s", val);
            else snprintf(msg, BUFFER_LENGTH, "NO EXIST");			
			break;
		case KVS_CMD_HDEL: 
			int res = kvstore_hash_delete(key);
			if (res < 0)  snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
            else if (res == 0) snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
            else snprintf(msg, BUFFER_LENGTH, "NO EXIST");
			break;
		case KVS_CMD_HMOD: 
			int res = kvstore_hash_modify(key, value);
			if (res < 0)  snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");	 
            else if (res == 0) snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
            else snprintf(msg, BUFFER_LENGTH, "NO EXIST");
			break;
		case KVS_CMD_HCOUNT: 
			int count = kvstore_hash_count();
			if (count < 0)   snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
            else snprintf(msg, BUFFER_LENGTH, "%d", count);
			break;
		default: 
			printf("cmd: %s\n", commands[cmd]);
			assert(0);
	}
}

// 收到recv的消息后进行解析，消息是一个命令字符串
void kvstore_request(struct conn_item * item){
	char *msg = item->rbuffer;
	char *tokens[KVSTORE_MAX_TOKENS];
	int count = kvstore_split_token(msg, tokens);
	kvstore_parser_protocol(item, tokens, count);
}
