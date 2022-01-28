#include "my_malloc.h"

/*
Qiangqiang Liu
Jan/15, 2022


malloc will be implemented with linked list data structure to bookkeeping the
status of heap memo
*/

//global variables
node_t * head = NULL;
node_t * tail = NULL;

unsigned long heap_size = 0;
unsigned long free_space = 0;

/* 
This function will implement the malloc function with the first fit policy
When finding the available space, we return the one that we first match.
*/
void * ff_malloc(size_t size) {
  //1. search in the LinkedList to find if there is any free space
  node_t * fit = first_fit(size);

  if (fit == NULL) {
    //we do not find a free node, we need allocate more heap space

    //totally we need alloc size + sizeof(node_t) space

    size_t total = size + sizeof(node_t);

    void * sbr = my_sbrk(total);

    if (sbr == NULL) {
      return NULL;
    }

    node_t * node = (node_t *)sbr;

    node->size = size;
    node->prev = NULL;
    node->next = NULL;

    return (sbr + sizeof(node_t));
  }
  else {
    //we have a fit, check if we need to split the node
    if ((fit->size - size) <= sizeof(node_t)) {
      //the node fit is too small to split
      //just remove the node from the free list
      removeLL(fit);
      free_space -= (sizeof(node_t) + fit->size);
      return (((void *)fit) + sizeof(node_t));
    }
    else {
      //split the node
      return splitNode(fit, size);
    }
  }

  return NULL;
}

void * splitNode(node_t * node, size_t size) {
  void * ptr = (void *)node;

  node_t * split = (node_t *)(ptr + sizeof(node_t) + size);

  //set split node
  split->size = node->size - size - sizeof(node_t);
  split->next = NULL;
  split->prev = NULL;

  node->size = size;

  if (head == node && tail == node) {
    head = split;
    tail = split;
  }
  else if (head == node) {
    split->next = head->next;
    head->next->prev = split;
    head = split;
  }
  else if (tail == node) {
    split->prev = tail->prev;
    tail->prev->next = split;
    tail = split;
  }
  else {
    split->prev = node->prev;
    split->next = node->next;

    node->prev->next = split;
    node->next->prev = split;
  }

  node->next = NULL;
  node->prev = NULL;

  free_space -= (size + sizeof(node_t));
  return (ptr + sizeof(node_t));
}

void ff_free(void * ptr) {
  my_free(ptr);
}

/*                                                                                   
This function will implement the malloc function with the first fit policy           
When finding the available space, we return the one that we first match.             
*/
void * bf_malloc(size_t size) {
  //1. search in the LinkedList to find if there is any free space
  node_t * fit = best_fit(size);

  if (fit == NULL) {
    //we do not find a free node, we need allocate more heap space

    //totally we need alloc size + sizeof(node_t) space

    size_t total = size + sizeof(node_t);

    void * sbr = my_sbrk(total);

    if (sbr == NULL) {
      return NULL;
    }

    node_t * node = (node_t *)sbr;

    node->size = size;
    node->prev = NULL;
    node->next = NULL;

    return (sbr + sizeof(node_t));
  }
  else {
    //we have a fit, check if we need to split the node
    if ((fit->size - size) <= sizeof(node_t)) {
      //the node fit is too small to split
      //just remove the node from the free list
      removeLL(fit);
      free_space -= (sizeof(node_t) + fit->size);
      return (((void *)fit) + sizeof(node_t));
    }
    else {
      //split the node
      return splitNode(fit, size);
    }
  }

  return NULL;
}

/*                                                                          
  this function will search through the LinkedList to 
  search the best fit node                                                   
                                                                            
  rerturn:                                                                  
  if there is: return the pointer to the node                               
  else:  return NULL                                                        

*/
node_t * best_fit(size_t size) {
  node_t * cur = head;

  int best = 2147483647;
  node_t * res = NULL;

  while (cur != NULL) {
    if (cur->size == size) {
      return cur;
    }

    if (cur->size > size && cur->size - size < best) {
      res = cur;
      best = cur->size - size;
    }

    cur = cur->next;
  }

  return res;
}

void bf_free(void * ptr) {
  my_free(ptr);
}

/*
  this function will search through the LinkedList to search the first node   
  that can fit the request
  
  rerturn:
  if there is: return the pointer to the node
  else:  return NULL
*/
node_t * first_fit(size_t size) {
  node_t * cur = head;
  while (cur != NULL) {
    if (cur->size >= size) {
      return cur;
    }
    cur = cur->next;
  }

  return cur;
}

int main(void) {
  //test the first fit
  node_t n1;
  node_t n2;
  node_t n3;

  n1.next = &n2;
  n2.next = &n3;
  n3.next = NULL;

  n3.prev = &n2;
  n2.prev = &n1;
  n1.prev = NULL;

  n1.size = 10;
  n2.size = 12;
  n3.size = 14;

  head = &n1;
  tail = &n3;
  printf("------asserts---------\n");

  assert(first_fit(10) == &n1);
  assert(first_fit(9) == &n1);
  assert(first_fit(0) == &n1);

  assert(first_fit(12) == &n2);
  assert(first_fit(11) == &n2);

  assert(first_fit(13) == &n3);
  assert(first_fit(14) == &n3);
  assert(first_fit(15) == NULL);

  /*test mysbrk*/
  void * cur = sbrk(0);
  size_t cur_heap_size = heap_size;
  void * res;
  //1
  assert(cur == my_sbrk(0));
  assert(heap_size == cur_heap_size);

  //2
  res = my_sbrk(10);
  assert(res == cur);
  cur += 10;
  cur_heap_size += 10;
  assert(cur == sbrk(0));
  assert(heap_size == cur_heap_size);

  //3
  res = my_sbrk(100);
  assert(res == cur);
  cur += 100;
  cur_heap_size += 100;
  assert(cur == sbrk(0));
  assert(heap_size == cur_heap_size);

  /* Test add tail */

  head = NULL;
  tail = NULL;

  addTail(&n1);
  assert(tail == &n1);
  assert(head == &n1);

  addTail(&n2);
  assert(tail == &n2);
  assert(head->next == &n2);

  addTail(&n3);
  assert(tail == &n3);
  assert(head == &n1);
  assert(head->next == &n2);
  assert(head->next->next == &n3);

  /*Test ff_alloc*/
  void * ff_cur = sbrk(0) + sizeof(node_t);
  node_t * ff_res;
  for (int i = 0; i < 10; i++) {
    ff_res = ff_malloc(100);
    assert(ff_cur == ff_res);

    ff_cur += (100 + sizeof(node_t));
  }

  /* Test remove */
  removeLL(&n2);
  assert(head == &n1);
  assert(tail == &n3);
  assert(head->next == tail);
  assert(tail->prev == head);
  assert(head->prev == NULL);
  assert(tail->next == NULL);

  removeLL(&n1);
  assert(head == &n3);
  assert(tail == &n3);

  assert(tail->next == NULL);
  assert(head->prev == NULL);

  removeLL(&n3);
  assert(head == NULL);
  assert(tail == NULL);

  /*test split the node*/

  printf("-------all passed--------\n");

  return -1;
}

/*                                                                   
my_sbrk is the same as sbrk, except that every time we call sbrk     
The change of the heap size will be accumulated into the global      
variable heap_size                                                   
*/

void * my_sbrk(intptr_t increment) {
  void * res = sbrk(increment);
  if (res == (void *)(-1)) {
    return NULL;
  }
  else {
    heap_size += increment;
    return res;
  }
}

/*
This function will help us free the allocated memo
1.When there is free node next, we will merge the free node with current node
2.When there is free node previous, we will merge this node with the previous node  
*/
void my_free(void * ptr) {
  //1. get the node
  node_t * node = (node_t *)(ptr - sizeof(node_t));

  if (head == NULL && tail == NULL) {
    head = node;
    tail = node;
    return;
  }

  //2. find the next free node after ptr
  node_t * next = findGreater(node);

  node_t * prev = NULL;

  free_space += (sizeof(node_t) + node->size);

  //3.
  if (next == NULL) {
    //check the tail
    prev = tail;
  }
  else {
    //find the prev node
    prev = next->prev;
  }

  if (prev == NULL && next == NULL) {
    head = node;
    tail = node;
    return;
  }

  //insert the node after prev
  insertLL(prev, node);

  //check for merge
  //1. try to merge with the next
  if (next != NULL && (void *)node + sizeof(node_t) + node->size == next) {
    //we can merge by add the size of next to node
    node->size += (next->size + sizeof(node_t));
    //then delete the next
    removeLL(next);
  }

  //2. try to merge with the prev
  if (prev != NULL && (void *)prev + sizeof(node_t) + prev->size == node) {
    prev->size += (node->size + sizeof(node_t));
    //then delete the node
    removeLL(node);
  }
}

/*
This function will insert node after target 
if target is NULL, then head and tail will be set to node
*/

void insertLL(node_t * target, node_t * node) {
  if (target == NULL) {
    node->next = head;
    head->prev = node;
    head = node;
  }
  else if (tail == target) {
    //we just need to add it to tail
    addTail(node);
  }
  else {
    node_t * next = target->next;
    node->next = next;
    node->prev = target;
    target->next = node;
    next->prev = node;
  }
}

/*
This function will loop through the linked list to find the freee node that are 
in the right and are just greater than node

*/
node_t * findGreater(node_t * node) {
  node_t * cur = head;
  while (cur != NULL && cur <= node) {
    cur = cur->next;
  }

  //now cur will just be NULL or a node thatis just greater than node
  return cur;
}

/*                                              
Return the entire head memo in bytes            
*/

unsigned long get_data_segment_size() {
  return heap_size;
}

/*                                              
Return the free space in the heap:              
usable free space + space occupied by meta-data 
                                                
*/

unsigned long get_data_segment_free_space_size() {
  return free_space;
}

void printLinkedList() {
  node_t * node = head;
  while (node != NULL) {
    printf("--------------------");
    printf("%p", node->prev);
    printf("%p", node->next);
    printf("--------------------");
    node = node->next;
  }
}

void addTail(node_t * node) {
  if (head == NULL || tail == NULL) {
    assert(tail == NULL);
    assert(head == NULL);
    head = node;
    tail = node;

    return;
  }

  assert(tail != NULL);

  node->prev = tail;
  tail->next = node;

  tail = node;
}

void removeLL(node_t * node) {
  if (head == NULL) {
    assert(tail == NULL);
    return;
  }

  if (node == head && node == tail) {
    head = NULL;
    tail = NULL;

    return;
  }

  if (node == head) {
    head = head->next;
    head->prev = NULL;
    node->next = NULL;
    return;
  }
  else if (node == tail) {
    tail = tail->prev;
    tail->next = NULL;
    node->prev = NULL;
    return;
  }

  node_t * pr = node->prev;
  node_t * nxt = node->next;

  pr->next = nxt;
  nxt->prev = pr;

  node->next = NULL;
  node->prev = NULL;

  return;
}
