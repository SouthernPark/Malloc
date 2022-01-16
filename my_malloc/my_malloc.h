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
This function will help use add the node to the tail of the Linked List
*/

void addToTail(node_t * n);

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

void * my_sbrk(intptr_t increment);

/*
This function will help us free the allocated memo
1.When there is free node next, we will merge the free node with current node
2.When there is free node previous, we will merge this node with the previous node  

*/
void free(void * ptr);

//next node will be merged into n node
void merge(node_t * n, node_t * next);
