#include<stdio.h>
#include<stdlib.h>

typedef struct meminfo{
  struct meminfo *next;
  size_t size;
}Meminfo;

void *my_malloc(size_t mem_size);
void my_free(void *);

char memory[1000000];
Meminfo *head = NULL;

void *my_malloc(size_t mem_size){
	
  if(mem_size < 1){
    return NULL;
  }
  
  if(head == NULL){
    Meminfo *newnode = (Meminfo *)memory;
    newnode->next = NULL;
    newnode->size = sizeof(memory) - sizeof(Meminfo);
    head = newnode;
  }

  void *addr = NULL;
  Meminfo *curptr = NULL;
  Meminfo *prevptr = NULL;
  curptr = prevptr = head;
 
  while(curptr != NULL){

    if(curptr->size >= (mem_size + (sizeof(Meminfo)))){
      if(curptr->size == mem_size){
        addr = curptr + 1;
        curptr->size = 0;
        prevptr->next = curptr->next;	
         return (void *)addr;
     	}
      else if(curptr->size > (mem_size + sizeof(Meminfo))){
	addr = curptr + 1 + (int) ((curptr->size - (mem_size + sizeof(Meminfo)))/ sizeof(Meminfo));
	curptr->size -= (mem_size + sizeof(Meminfo));
 	
	Meminfo *newnode = (Meminfo *)addr;
	newnode->size = mem_size;
	newnode->next = NULL;
	addr = newnode + 1;
        
      return (void *)addr;
      }
    }

    prevptr = curptr;
    curptr = curptr->next;
      
  }
 
  return NULL;
}


void my_free(void *memloc){
  
  if((int *)memloc == NULL){
    return;
  }

  Meminfo *node =  (Meminfo *)memloc - 1; 
  node->next = head;
  head = node; 

}

void display_list(){
  Meminfo * temp = head;
  while(temp != NULL){
    printf("size %ld\n", temp->size);
   temp = temp->next;
  }
}


int main(){

  int *ptr = (int *)my_malloc(4 * sizeof(int)); 
 
  if(ptr){
    ptr[0] = 1;
    ptr[1] = 2;
    ptr[2] = 3;
    ptr[3] = 4;
    printf("val : %d %d %d %d\n", ptr[0], ptr[1], ptr[2], ptr[3]);
  }
  
  int *ptr2 = (int *)my_malloc(3* sizeof(int));
  if(ptr2){
    ptr2[0] = 33;
    ptr2[2] = 22;
    ptr2[1] = 11;

    printf("val : %d %d %d\n", ptr2[0], ptr2[1], ptr2[2]);
  }
  my_free(ptr);
  ptr = (int * )my_malloc(13 * sizeof(int));
  if(ptr){
    ptr[0] = 22;
    ptr[1] = 1;
    ptr[12] = 66;

    printf("val : %d %d %d\n", ptr[0], ptr[1], ptr[12]);
  }

  char * c1 = (char *)my_malloc(5 * sizeof(char));
//display_list();
      	char * c2 = (char *)my_malloc(1 * sizeof(char));
//display_list();
      	if(c1){
   c1[3] = '3';
   printf("val : %c\n", c1[3]);
  }

  if(c2){
    c2[0] = 'a';
    printf("val : %c\n", c2[0]);
  }

  my_free(c1);
//display_list();
  c1 = (char *)my_malloc(30* sizeof(char));
  if(c1){
   c1[1] = 'y';
   printf("val : %c\n", c1[1]);
  }
//display_list();
  char *c3 = (char *)my_malloc(4 * sizeof(char));
  if(c3){
   c3[3] = 'n';
   printf("val : %c\n", c3[3]);
  }
 // display_list();
  return 0;
}


