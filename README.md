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
    
    
    Suppose, the user want to use malloc to allocate for 8 bytes of memory. May be we will allocate 10 bytes memo using system call
    From the 10 bytes, 8 bytes will be used for the user and another 2 bytes will be used as bookkeeping to record this allocation.
    

Thirdly, because bookkeeping record is attached with requests. This means they can not be stored consecutively. Therefore, we can not  
data structure like array to store it. A good data structure to fit solve this problem is Linked List or we can even use double Linked  
List.



