# Malloc
Malloc library will be implemented with C by using the system call sbrk


    void * malloc(size_t size):
      parameter: size how many bytes to allocate
      return: a pointer to the new allocated memory

# Idea  
Firstly, we need bookkeeping to record the status of allocated memory and whether that allocated memo has been freed.  
Nothing happens for granted. Bookkeeping also needs some memory. This means except from allocating memo for the request,  
we still need allocate more memo for the bookkeeping.  



Secondly, it is very hard to allocate a specific number of memo for bookkeeping at the very beginning. Because we actually  
don't know how many malloc requests there will be. Therefore, a common thinking will be the record of a particular request  
will be attached with memo we want to relloc. Here I will give you an example:  
    
    
    Suppose, the user want to use malloc to allocate for 8 bytes of memory. May be we will allocate 10 bytes memo using 
    system call. From the 10 bytes, 8 bytes will be used for the user and another 2 bytes will be used as bookkeeping 
    to record this allocation.  
    

Thirdly, because bookkeeping record is attached with requests. This means they can not be stored consecutively. Therefore, we can not  
data structure like array to store it. A good data structure to fit solve this problem is Linked List or we can even use double Linked  
List.


# LinkedList in C
The node of the linked list should hold some variables:  


        1.  the pointer to the next node  
        2.  the pointer to the prev node  
        3.  an integer specify how much bytes this block has been assigned to the   
        4.  an boolean value that specify whether this block of memo has been occupied  
        
The struct can be written as:  
        
        
        type struct node_tag{  
            node_t* next;  
            node_t* prev;  
            size_t size;  
            char used;  
        } node_t;   

We also need some pointer to at the start of the linked list. We can use a global variable to denote the head of the linked list.  
We can initially set the pointer as NULL. We we request malloc for the first time, we knwo that the pointer is null. Then we assign  
it with the current program break. And do the allocating staff, for the following requests because the pointer is not null, we does  
not change it.  
    
        
        node_t * head;       //at global variable
        node_t * tail;
        

# Implement 


## malloc
        
        //define the struct as above  
        node_t * head;  
        node_t * tail;
        void* malloc(size_t size){  
            //check whether head is null
            if(head == null){
                //find the current program break using linux system call sbrk()
                head = sbrk(0);
            }
            
            //allocate memo for the nodes and memo bytes requested by the user
            
            //1. calculate the total bytes memo needed
            
            //2. search in the linked list to check fit
                //if there is fit, we can use that fit
                    //if the aviable memo is greater than the request, we have to split 
                    //if it is equal, it is ok and do nothing
                    
                    //also do not forget set the used status to 1
            
                //else if there is no fit in the linked list
                    //we have to using sbrk to allocate that amount the space
                    //add the record to the linked list
                    
                    //also do not forget to set the used status to 1
            
            //3. return the address of the allocated memo 
        }  
    
        -------
        We want to use sbrk() as less as possible, because we want to keep the size of the heap as small as possible. This means
        we have to use bookkeeping to check if there is a fit already in the heap. When we find the fit, we might have to split the 
        node. But there are some conner cases when spliting the nodes.  
        Corner Case:
        For example, the user want to request a 3 bytes memo in the heap. We find a fit with 4 bytes in the bookkeeping. Let us assume
        size of the node of the linked list is 2 bytes.
        Record: 2B(node)|4B(available memo).
        Now we have:  
        2B(node)|3B(occupied)|1B(available).  
        The last 1B available can not be recorded, because it can not even fit the node.  
        The remaining free space is too small to keep track of.
        
    

## free

    void free(void * ptr){
        //1. using ptr to get the linked list record
            //ptr - sizeof(node_t)
        //2. check whether the prev node and next node is free
            //if it is, we have to merge the nodes into one node
            //else, set the used status to 0
    }


# Peformance test

We are gonna to use framentation to test the space performance of other algorithm. Therefore, we have to know two informations:  

    1. the entire heap memo
            unsigned long get_data_segment_size();  //in bytes
            
    2. free space: which is the free space in the heap and the memo used for the meta data
            unsignedlongget_data_segment_free_space_size();     //inbytes 
            
## entire heap memo  
We could maintain a global which will hold the entire heap memo to date. Every time we use sbrk() we are gonna to add the changes
to this global variable.

## free space:
free space comes from the fact that:  
    1. meta-data: which is the size of nodes.  
            Every time we create, merge and split nodes, we have to conside about change the free space
    2. when we call free(), we deacllocate some memo and this will count to free space.


# Reference:

## sbrk
used to change the data segment size of the program.  

    void * sbrk(intptr_t increment);  
    On success, sbrk() returns the previous program break. (If the break was increased,  
    then this value is a pointer to the start of the newly allocated memory).  
    
    
    On error, (void *) -1 is returned, and errno is set to ENOMEM.  

    intptr_t: signed integer type capable of holding a pointer to void.  
    

# Misunderstanding

## sbrk()
    
    sbrk() returns the previous program break.  
    The previous means the address before we move the program break.  

## struct padding

    The structure of node_t is:
    
    type struct node_tag{       
        node_t* next;   (8 bytes)           
        node_t* prev;   (8 bytes)        
        size_t size;    (8 bytes)        
        char used;      (1 bytes)        
    } node_t;                   
    
    
    But the actually of node_t is 32 bytes.  
    
    The last field is padded to alig with the other fields to become some  
    kind od rectangle like fields.  

## pointer arithmetic
    
    assume we have node_t * ptr, we want to move to forward 8 bytes.
    ptr -= 8 is not correct, because the movement unit now is sizeof(node_t).
    The sizeof(node_t) is 32. This means we actually moves for 8*32 bytes forward.
    
    To fix this we have to firstly convert the node_t * to char *  
    After bytes moving, we convert the pointer back.
    
    We can create a function do this dirty thing.  
    
    
# Improvement

With the increase of node, we have to loop through the linkedlist to find the fit.  
During the traversal, we may find that most of the nodes are actually used.  
This will waste some time. A good way will be keep a list that will recorded the  
nodes that are freed.  
    
    
Therefore, we can have the following data structure:


    type struct node_tag{                       
        node_t* next;   (8 bytes)               
        node_t* prev;   (8 bytes)               
        node_t * free_next (8 bytes)            
        node_t * free_prev (8 bytes)            
        size_t size;    (8 bytes)               
        char used;      (1 bytes)               
    } node_t;                                   
    
    we will use global pointer free_head and free_tail to record the list.  
    
    
    
    
