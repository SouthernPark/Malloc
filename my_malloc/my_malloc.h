/*                                                                                  
Qiangqiang Liu                                                                      
Jan/15, 2022                                                                        
                                                                                    
                                                                                    
malloc will be implemented with linked list data structure to bookkeeping the       
status of heap memo                                                                 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_INT 2147483647
#include <assert.h>

//first fit
void * ff_malloc(size_t size);

void ff_free(void * ptr);

//best fit

void * bf_malloc(size_t size);

void bf_free(void * ptr);

/* Node data structure for Double Linked List*/

//create node structure
typedef struct node_tag {
  struct node_tag * next;
  struct node_tag * prev;

  //how many bytes requested by the user
  size_t size;

} node_t;

/* Anxiliary Function */

/*                                                                          
  this function will search through the LinkedList to search the first node 
  that can fit the request                                                  
                                                                            
  rerturn:                                                                  
  if there is: return the pointer to the node                               
  else:  return NULL                                                        

*/
node_t * first_fit(size_t size);

/*                                                                          
  this function will search through the LinkedList to 
  search the best fit node                                                   
                                                                            
  rerturn:                                                                  
  if there is: return the pointer to the node                               
  else:  return NULL                                                        

*/
node_t * best_fit(size_t size);

/*
my_sbrk is the same as sbrk, except that every time we call sbrk
The change of the heap size will be accumulated into the global
variable heap_size
*/

void * my_sbrk(intptr_t increment);

/*
This function will help us free the allocated memo
1.When there is free node next, we will merge the free node with current node
2.When there is free node previous, we will merge this node with the previous node  

*/
void my_free(void * ptr);

/*
Return the entire head memo in bytes
*/

unsigned long get_data_segment_size();

/*
Return the free space in the heap:
usable free space + space occupied by meta-data
*/
unsigned long get_data_segment_free_space_size();

void printLinkedList();

void addTail(node_t * node);

void removeLL(node_t * node);

void insertLL(node_t * target, node_t * node);

void * splitNode(node_t * node, size_t size);

node_t * findGreater(node_t * node);
