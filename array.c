#include "kvstore.h"

#define ARRAY_SIZE 1024

struct array_item{
	char *key;
	char *value;
};

typedef struct array_s{
	struct array_item *array_table;
	int array_idx;
}array_t;

array_t Array;


int array_create(array_t * arr){
	if(!arr) return -1;
	arr->array_table = kvstore_malloc(ARRAY_SIZE * sizeof(struct array_item));
	if(!arr->array_table) return -1;
	memset(arr->array_table, 0, ARRAY_SIZE * sizeof(struct array_item));
	arr->array_idx = 0;
	return 0;
}

void array_destory(array_t * arr){
	if(!arr) return;
	if(!arr->array_table) kvstore_free(arr->array_table);
}

int array_set(array_t * arr, char * key, char * value){
	if(arr == NULL || key == NULL || value == NULL) return -1;
	if(arr->array_idx == ARRAY_SIZE) return -1;

	char *keycopy = kvstore_malloc(strlen(key)+1);
	if(keycopy == NULL) return -1;
	// 将字符串 key 的内容复制到另一个字符串 keycopy 中，长度为strlen(key)+1
	strncpy(keycopy, key, strlen(key)+1);
	char *valuecopy = kvstore_malloc(strlen(value) + 1);
	if (valuecopy == NULL) {
		kvstore_free(keycopy);
		return -1;
	}
	strncpy(valuecopy, value, strlen(value)+1);

	int i = 0;
	// 找到第一个空槽位，将键值对存储在这个位置，并将数组的索引增加 1。
	for(; i < arr->array_idx; ++i){
		if (arr->array_table[i].key == NULL) {
			arr->array_table[i].key = keycopy;
			arr->array_table[i].value = valuecopy;
			arr->array_idx ++;
			return 0;
		}
	}
	// 如果数组中没有空槽位，但数组还未满，则将键值对存储在数组的末尾，并将数组的索引增加 1。
	if(i < ARRAY_SIZE && i == arr->array_idx){
		arr->array_table[arr->array_idx].key = keycopy;
		arr->array_table[arr->array_idx].value = valuecopy;
		arr->array_idx++;
	}
	return 0;
}

char *array_get(array_t * arr, char * key){
	int i = 0;
	if(arr == NULL) return NULL;
	for(i = 0; i < arr->array_idx; ++i){
		if(arr->array_table[i].key == NULL) return NULL;
		if(strcmp(arr->array_table[i].key, key) == 0) return arr->array_table[i].value;
	}
	return NULL;
}

int array_delete(array_t * arr, char * key){
	int i = 0;
	if (arr == NULL || key == NULL) return -1;
	for(; i < arr->array_idx; ++i){
		if(strcmp(arr->array_table[i].key, key) ==     0){
			kvstore_free(arr->array_table[i].value);
			arr->array_table[i].value = NULL;
			kvstore_free(arr->array_table[i].key);
			arr->array_table[i].key = NULL;
			arr->array_idx--;
			return 0;
		}
	}
	return i; // i > 0 表示不存在这个key
}

int array_modify(array_t * arr, char * key, char * value){
	int i = 0;
	if(arr == NULL || key == NULL || value == NULL) return -1;
	for(; i < arr->array_idx; ++i){
		if(strcmp(arr->array_table[i].key, key) == 0){
			kvstore_free(arr->array_table[i].value);
			arr->array_table[i].value = NULL;
			char *valuecopy = kvstore_malloc(strlen(value)+1);
			arr->array_table[i].value = valuecopy;
			return 0;
		}
	}
	return i; // i > 0 表示不存在这个key
}

int array_count(array_t * arr){
	if(!arr) return -1;
	return arr->array_idx;
}

