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

  struct node_tag * free_next;
  struct node_tag * free_prev;

  //how many bytes requested by the user
  size_t size;
  //1-> these bytes are used, 0-> available for use
  int used;
} node_t;

/* Anxiliary Function */

/*
This function will allocate new space to hold the node    
return the pointer to the node                            

*/
node_t * makeSpaceForNode();

/*
this function is used when malloc is called for the first time
we init the Linked List as the current program break          

*/
void * initLL(size_t size);

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
this function will be called when there is no fit found in the heap 
and we have to increase the heap to give the user requested memo    
return the address of the space requested by the user               

*/
void * incr_heap(size_t size);

/*
This function will help use add the node to the tail of the bookkeeping Linked List
*/

void listAddToTail(node_t * n);

/*                                           
Insert node n after target                   
*/

void listInsert(node_t * n, node_t * target);

/*
rRemove the node from the bookkeeping linked list
*/
void listRemove(node_t * n);

/*                                                   
Replace the node target in the free List with node n 
*/

void freeListReplace(node_t * n, node_t * target);

/*                               
Remove node n from the free list 
*/

void freeListRemove(node_t * n);

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
void * splitNode(node_t * n, size_t size);

/*
my_sbrk is the same as sbrk, except that every time we call sbrk
The change of the heap size will be accumulated into the global
variable heap_size
*/

void * my_sbrk(int increment);

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

void printFreeLinkedList();
