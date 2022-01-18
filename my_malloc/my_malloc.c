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
  //1. check if empty (check whether the Linked List is null)
  if (head == NULL) {
    //intially set the head and tail of the linked list

    return initLL(size);
  }

  //2. search for the first fit
  node_t * first = first_fit(size);
  if (first != NULL) {
    //split the first matched node
    //and return the address of space that the user requested
    return splitNode(first, size);
  }

  //else first == NULL, which means there is no fit
  //we have to increase the heap
  void * address = incr_heap(size);
  return address;
}

void ff_free(void * ptr) {
  my_free(ptr);
}

void * bf_malloc(size_t size) {
  //1. check if empty (check whether the Linked List is null)
  if (head == NULL) {
    //intially set the head and tail of the linked list
    return initLL(size);
  }

  //2. search for the best fit
  node_t * best = best_fit(size);
  if (best != NULL) {
    //split the first matched node
    //and return the address of space that the user requested
    return splitNode(best, size);
  }

  //else first == NULL, which means there is no fit
  //we have to increase the heap
  void * address = incr_heap(size);
  return address;
}

/*                                                                          
  this function will search through the LinkedList to 
  search the best fit node                                                   
                                                                            
  rerturn:                                                                  
  if there is: return the pointer to the node                               
  else:  return NULL                                                        
*/
node_t * best_fit(size_t size) {
  if (head == NULL) {
    return NULL;
  }

  node_t * best = NULL;
  int difference = MAX_INT;

  node_t * cur = head;
  while (cur != NULL) {
    if (cur->used == 0 && cur->size == size) {
      return cur;
    }
    else if (cur->used == 0 && cur->size > size && cur->size - size < difference) {
      best = cur;
      difference = cur->size - size;
    }
    cur = cur->next;
  }

  return best;
}

void bf_free(void * ptr) {
  my_free(ptr);
}

//This function will allocate new space to hold the node
//return the pointer to the node
node_t * makeSpaceForNode() {
  //1. calling the sbrk to allocate sizeof(node_t) bytes
  void * prev_brk = my_sbrk(sizeof(node_t));

  free_space += sizeof(node_t);  //node space is counted as free space

  //2. return the pointer to the beginning of the node
  return (node_t *)prev_brk;
}

//this function is used when malloc is called for the first time
//we init the Linked List as the current program break
void * initLL(size_t size) {
  //1. make space for the node
  node_t * n = makeSpaceForNode();

  //2. make space for the request
  void * res = my_sbrk(size);
  //3. set up the node
  n->prev = NULL;
  n->next = NULL;
  n->size = size;
  n->used = 1;

  head = n;
  tail = n;

  return res;
}

/*
  this function will search through the LinkedList to search the first node   
  that can fit the request
  
  rerturn:
  if there is: return the pointer to the node
  else:  return NULL
*/
node_t * first_fit(size_t size) {
  if (head == NULL) {
    return NULL;
  }

  node_t * cur = head;

  while (cur != NULL) {
    if (cur->used == 0 && cur->size >= size) {
      //return the first fit
      return cur;
    }
    //else move to the next node
    cur = cur->next;
  }
  //we are here beacause cur == NULL, which means there is no fit

  return cur;
}

/*
this function will be called when there is no fit found in the heap   
and we have to increase the heap to give the user requested memo      
return the address of the space requested by the user                 
*/
void * incr_heap(size_t size) {
  //1. make space for node
  node_t * n = makeSpaceForNode();

  //2. make space for the user
  void * res = my_sbrk(size);
  n->size = size;
  n->used = 1;

  //3. add the node to the tail of Linked List
  addToTail(n);

  //4. return the address requested by the user
  return res;
}

void addToTail(node_t * n) {
  n->prev = tail;
  tail->next = n;
  n->next = NULL;
  tail = n;
}

/*                                                                            
Because we find a matched space in the linkedlist, we now have to give the    
space the user requested. Here are two cases:                                 
    1. After we split the node, the rest of the space is too small to record  
       In this situation, we just give the user the whole space, which means  
       We do not split the node.                                              
    2. Else the rest of the node is still very big, we can split the node     
       And set the rest of the node as available.                             
                                                                              
    return: return the adrress of the space that the user requested.          
*/
void * splitNode(node_t * n, size_t size) {
  //1. check whether the splited node is too small to record
  if (n->size - size > NODE_SIZE) {
    //we can record the splited node

    //get the pointer of the splited node
    node_t * split = (node_t *)((char *)n + size + NODE_SIZE);
    //node_t * split = (node_t *)(ptrByteMove(n, sizeof(node_t) + size, 1));
    //insert split between n and n->next
    split->prev = n;
    split->next = n->next;

    n->next = split;
    if (split->next == NULL) {
      //split now is the tail
      tail = split;
    }
    else {
      //split is not the tail, change the prev pointer of its next node
      split->next->prev = split;
    }

    //change the size and used property of n and split
    split->size = n->size - size - NODE_SIZE;
    split->used = 0;  //split is free for use
    n->size = size;
    n->used = 1;  //n is now being used

    free_space -= size;
  }
  else {
    //first case: do not need to split
    n->used = 1;
    free_space -= n->size;
  }

  //return the address the user requests
  //return ptrByteMove(n, sizeof(node_t), 1);
  return (void *)((char *)n + NODE_SIZE);
}

/*                                                                   
my_sbrk is the same as sbrk, except that every time we call sbrk     
The change of the heap size will be accumulated into the global      
variable heap_size                                                   
*/

void * my_sbrk(intptr_t increment) {
  heap_size += increment;

  return sbrk(increment);
}

/*
This function will help us free the allocated memo
1.When there is free node next, we will merge the free node with current node
2.When there is free node previous, we will merge this node with the previous node  
*/
void my_free(void * ptr) {
  //1. Get the corresponding node pointer
  //node_t * n = (node_t *)ptrByteMove(ptr, sizeof(node_t), -1);
  node_t * n = (node_t *)((char *)ptr - NODE_SIZE);
  //2. set the status of the node to unused
  n->used = 0;
  //because node n is freed, increase the free space
  free_space += n->size;

  //3. check whether the next node is free
  node_t * next = n->next;
  if (next != NULL && next->used == 0) {
    //merge the next node into node n
    merge(n, next);
  }

  //4. check whether the previous node is free
  node_t * prev = n->prev;
  if (prev != NULL && prev->used == 0) {
    //merge node n into prev
    merge(prev, n);
  }
}

//next node will be merged into n node
void merge(node_t * n, node_t * next) {
  //a. merge the next node with current node
  n->size = n->size + sizeof(node_t) + next->size;

  //b. remove next node from the linked list
  if (next->next == NULL) {
    //next is the tail
    n->next = NULL;
    tail = n;
  }
  else {
    //next is not the tail
    n->next = next->next;
    next->next->prev = n;
  }

  //free_space deos not change, because the removed node
  //is also free space
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
