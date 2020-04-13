#ifndef __hash_table_H
#define __hash_table_H

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

#endif 
