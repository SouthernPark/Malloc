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
    if (cur->size == size) {
      return cur;
    }
    else if (cur->size > size && cur->size - size < difference) {
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

  node_t * n = (node_t *)prev_brk;

  //2. set up the node

  n->prev = NULL;
  n->next = NULL;

  //3. return the pointer to the beginning of the node
  return n;
}

//this function is used when malloc is called for the first time
//we init the Linked List as the current program break
void * initLL(size_t size) {
  //1. make space for the node
  node_t * n = makeSpaceForNode();

  //2. make space for the request
  void * res = my_sbrk(size);

  //3. set the properties of the node
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
  //search in the free list
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

  n->size = size;
  n->used = 1;

  //2. make space for the user
  void * res = my_sbrk(size);

  //3. return the address requested by the user
  return res;
}

void listAddToTail(node_t * n) {
  if (head == NULL) {
    head = n;
    tail = n;
    return;
  }
  n->prev = tail;
  tail->next = n;
  n->next = NULL;
  tail = n;
}

/*                                                  
rRemove the node from the bookkeeping linked list   
*/
void listRemove(node_t * n) {
  if (n == head && n == tail) {
    head = NULL;
    tail = NULL;
  }
  else if (n == head) {
    head = head->next;
    head->prev = NULL;
    n->next = NULL;
  }
  else if (n == tail) {
    tail = tail->prev;
    tail->next = NULL;
    n->prev = NULL;
  }
  else {
    n->prev->next = n->next;
    n->next->prev = n->prev;
  }
}

/*
Insert node n after target
*/

void listInsert(node_t * n, node_t * target) {
  if (target == tail) {
    //target is the tail
    listAddToTail(n);
  }
  else {
    //target is not the tail
    n->prev = target;
    n->next = target->next;
    target->next->prev = n;
    target->next = n;
  }
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
  if (n->size - size > sizeof(node_t)) {
    //We can split

    //a. get the pointer of the splited node
    // node_t * split = (node_t *)(ptrByteMove(n, sizeof(node_t) + size, 1));
    node_t * split = (node_t *)((char *)n + sizeof(node_t) + size);

    //c. change the size and used property of n and split

    split->size = n->size - size - sizeof(node_t);
    split->used = 0;  //split is free for use
    n->size = size;
    n->used = 1;  //n is now being used

    //e. add to the tail of list
    listAddTail(split);

    free_space -= size;
  }
  else {
    //Too small to split

    //1. set to used
    n->used = 1;

    //2. remove free node n from the list
    listRemove(n);
    free_space -= n->size;
  }

  //return the address the user requests
  return (node_t *)((char *)n + sizeof(node_t));
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

  node_t * n = (node_t *)((char *)ptr - sizeof(node_t));

  //2. set the status of the node to unused

  //because node n is freed, increase the free space
  free_space += n->size;

  //3. check whether the free_head is NULL
  if (head == NULL) {
    n->used = 0;
    head = n;
    tail = n;
    return;
  }

  //4. check whether the prev node and next node is free

  node_t * next = n->next;
  node_t * prev = n->prev;

  int merge_prev = 1;

  if (prev == NULL) {
    merge_prev = -1;
  }
  else {
    //free prev and n are not connected
    if ((char *)prev + sizeof(node_t) + prev->size != n) {
      merge_prev = -1;
    }
  }

  int merge_next = 1;

  if (next == NULL) {
    merge_next = -1;
  }
  else {
    //free prev and n are not connected
    if ((char *)next - n->size - sizeof(node_t) != n) {
      merge_prev = -1;
    }
  }

  //can not merge
  if (merge_next == -1 && merge_prev == -1) {
    //add the free node to the end of free list
    n->used = 0;
    ListAddTail(n);
    return;
  }

  //3. check whether the next node is free

  if (merge_next != -1) {
    //merge the next node into node n
    n->size = n->size + sizeof(node_t) + next->size;
    // remove the next node from book keeping
    listRemove(next);
    n->used = 0;
  }

  //4. check whether the previous node is free

  if (merge_prev != -1) {
    //merge node n into prev
    prev->size = prev->size + sizeof(node_t) + n->size;
    //remove node n from book keeping
    listRemove(n);

    //bacuse we merge a node, then free space will be added
    free_space += sizeof(node_t);
  }
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
  return 100;
}
