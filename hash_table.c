#include <stdio.h> 
#include <stdlib.h> 
#include <assert.h>

#define R 97      // @Note: Small prime see hash string algoritm
#define M 5       // @Note: Length of hash table
#define true  1 
#define false 0 

typedef struct H_ITEM {
	int key;
	int val;
	struct H_ITEM *next;
	struct H_ITEM *prev;
} H_ITEM;

typedef struct H_TABLE {
	// These variables currently don't have any purpose 
	// but will when I add resizing later.
	int count; 
	int size;
	H_ITEM **table_arr;
} H_TABLE;

unsigned insert(H_TABLE *table, int key, int val);

void *xmalloc(size_t num_bytes) {
	void *ptr = malloc(num_bytes);
	if (!ptr) {
		perror("xmalloc failed");
		exit(1);
	}
	return ptr;
}

// @Note: Hash fuction taken from stb.h
unsigned int string_hash(H_TABLE *table, char *str) {
	unsigned int hash = 0;
	while (*str) {
		hash = (hash << 7) + (hash >> 25) + *str++;
	}
	return hash + (hash >> 16) % table->size;
}

// @Note: Hash fuction taken from stb.h
unsigned int string_len_hash(H_TABLE *table, char *str, int len) {
	assert(table->size > 0);
	unsigned hash = 0;
	while ((len--) > 0 && *str) {
		hash = (hash << 7) + (hash >> 25) + *str++;
	}
	return hash + (hash >> 16) % table->size;
}

// @Note: Hash fuction taken from stb.h
unsigned int hash_int(int size, unsigned int hash) {
	assert(size > 0);
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;
	return hash % size;
}

unsigned search(H_TABLE *table, int key) {
	assert(table->size > 0);
	unsigned hash_val = hash_int(table->size, key);
	//printf("hash_val = %d\n", hash_val);
	H_ITEM *current = table->table_arr[hash_val];
	if (current == NULL) {
		printf("No item found key %d\n", key);
		return false;
	}
	// @Cleanup: I dont need to do this 
	// since we are just moving the pointers 
	// Delete this unnecessary
	if (current->key == -1) {
		printf("No item found key %d\n", key);
		return false;
	}
	while (current != NULL) {
		if (current->key == key) {
			printf("Found key %d\n", key);
			return true;
		}
		else {
			current = current->next;
		}
	}
	printf("No item found with key %d\n", key);
	return false;
}

unsigned search_and_insert(H_TABLE *table, int key, int val) {
	if (search(table, key) == 0) {
		table->count++;
		assert(table->count >= 0);
		return insert(table, key, val);
	}
	else {
		printf("Found key=%d in hash table", key);
		return true;
	}
}

unsigned insert(H_TABLE *table, int key, int val) {
	assert(table->size > 0);
	H_ITEM *item = (H_ITEM*)malloc(sizeof(H_ITEM));
	item->key = key;
	item->val = val;
	item->prev = NULL;
	item->next = NULL;
	unsigned hash_val = hash_int(table->size, key);
	H_ITEM *current = table->table_arr[hash_val];
	H_ITEM *previous = NULL;
	if (current == NULL) {
		table->table_arr[hash_val] = item;
		table->count++;
		assert(table->count >= 0);
		return true;
	}
	while (current != NULL) {
		if (current->key == key) {
			printf("Pair key=%d, val=%d already exists replacing with key=%d val=%d\n", current->key, current->val, key, val);
			current->val = val;
			return true;
		}
		else {
			previous = current;
			current = current->next;
		}
	}
	current = item;
	current->prev = previous;
	previous->next = current;
	table->count++;
	assert(table->count >= 0);
	return true;
}

unsigned delete_entry(H_TABLE *table, int key) {
	assert(table->size > 0);
	unsigned hash_val = hash_int(table->size, key);
	H_ITEM *current = table->table_arr[hash_val];
	H_ITEM *previous = NULL;
	if (current == NULL) {
		printf("Attempt to delete non existent key=%d in hash table\n", key);
		return false;
	} 
	// @Note: This is ugly and there is a better way to do this 
	// I will look for a better solution when I'm not so busy! 
	while (current != NULL) {
		if (current->key == key) {
			// If we are in the middle of the list 
			if (current->prev != NULL && current->next != NULL) {
				previous->next = current->next;
				current->next->prev = current->prev;
				current->prev = NULL;
				current->next = NULL;
				current = NULL;
				table->count--;
				assert(table->count >= 0);
				return true;
				// We are at the end of the list
			}
			else if (current->prev != NULL && current->next == NULL) {
				previous->next = NULL;
				current->prev = NULL;
				// Should already be NULL but just to be safe 
				current->next = NULL;
				current = NULL;
				table->count--;
				assert(table->count >= 0);
				return true;
				// We are at the beginning of the list
			}
			else if (current->prev == NULL && current->next != NULL) {
				H_ITEM *temp = current->next;
				current->next = NULL;
				current->key = temp->key;
				current->val = temp->val;
				current->next = temp->next;
				current->prev = NULL;
				table->count--;
				assert(table->count >= 0);
				return true;
			}
			// There is only one element in the list
			else if (current->prev == NULL && current->next == NULL) {
				table->table_arr[hash_val] = NULL;
				table->count--;
				assert(table->count >= 0);
				return true;
			}
		}
		previous = current;
		current = current->next;
	}
	printf("Attempt to delete non existent key=%d in hash table\n", key);
	return false;
}

H_TABLE *initialize_hash_table(void) {
	H_TABLE *table = (H_TABLE*)malloc(sizeof(H_ITEM));
	table->count = 0;
	table->size = 5;
	table->table_arr = calloc((size_t)table->size, sizeof(H_ITEM*));
	return table;
}

void search_test(void) {
	H_TABLE *table = initialize_hash_table();
	H_ITEM *item1 = (H_ITEM*)malloc(sizeof(H_ITEM));
	item1->key = 3;
	item1->val = 30;
	item1->prev = NULL;
	item1->next = NULL;

	H_ITEM *item2 = (H_ITEM*)malloc(sizeof(H_ITEM));
	item2->key = 4;
	item2->val = 40;
	item2->prev = NULL;
	item2->next = NULL;

	item1->next = item2;
	item2->prev = item1;

	table->table_arr[3] = item1;

	H_ITEM *item3 = (H_ITEM*)malloc(sizeof(H_ITEM));
	item3->key = 25;
	item3->val = 250;
	item3->prev = NULL;
	item3->next = NULL;

	table->table_arr[3] = item1;
	table->table_arr[4] = item3;

	search(table, 3);
	search(table, 4);
	search(table, 25);

	free(table->table_arr);
	free(table);
	free(item1);
	free(item2);
	free(item3);
}

void insert_test(void) {
	H_TABLE *table = initialize_hash_table();
	insert(table, 3, 30);
	search(table, 3);
	insert(table, 4, 40);
	search(table, 4);
	insert(table, 3, 60);
	insert(table, 1, 80);
	insert(table, 9, 14);
	search(table, 1);
	search(table, 5);
	insert(table, 4, 5);
	search(table, 3);
	search(table, 4);
	search(table, 5);
	free(table);
}

void delete_test(void) {
	H_TABLE *table = initialize_hash_table();
	insert(table, 3, 30);
	search(table, 3);
	insert(table, 4, 40);
	delete_entry(table, 3);
	delete_entry(table, 4);
	search(table, 4);
	insert(table, 0, 0);
	insert(table, 1, 10);
	insert(table, 2, 20);
	delete_entry(table, 1);
	search(table, 0);
	search(table, 1);
	search(table, 2);
	free(table);
}

void tests(void) {
	search_test();
	insert_test();
	delete_test();
}

int main(int argc, char **argv) {
	H_TABLE *table = initialize_hash_table();
	//printf("%d\n", hash_int(table->size, 0));
	//printf("%d\n", hash_int(table->size, 1));
	//printf("%d\n", hash_int(table->size, 2));
	/* printf("%d\n", hash_int(table->size, 3)); */
	/* printf("%d\n", hash_int(table->size, 123)); */
	/* printf("%d\n", hash_int(table->size, 54)); */
	/* printf("%d\n", hash_int(table->size, 5)); */
	tests();
	printf("%s\n", "Success!");
	return 0;
}
