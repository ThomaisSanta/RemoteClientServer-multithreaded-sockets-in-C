#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>                         
#include <sys/socket.h> 
#include <sys/stat.h>                       
#include <netinet/in.h> 
#include <unistd.h>
#include <fcntl.h>              
#include <netdb.h>                       
#include <arpa/inet.h>
#include <pthread.h>
#include <dirent.h>
#include "queue.h"

int start_queue=0 ; //global variable in order to know if queue has been used

void insert_in_queue(char *file,int arg_socket, my_queue* queue) {
    pthread_mutex_lock(&queue_type.mutex) ; //so that no one else can have access to the queue
    if(start_queue==0) {  //queue has never been used before so initialize it
        queue->first_node = NULL ;
        queue->last_node = NULL ;
        queue_type.entries_in_queue = 0 ; //zero entries in queue so far
        start_queue=1 ; //now queue has been initialized
    }
    while(queue_type.entries_in_queue>=queue_type.queue_size) { //queue is full
        pthread_cond_wait(&queue_type.cond_comm,&queue_type.mutex) ;
    }
    queue_type.entries_in_queue++ ; //add a new entry so increase counter of entries
    my_node *new_node=NULL ;
    new_node = malloc(sizeof(my_node)) ;
    if(new_node==NULL) {
        perror("failed to create new node\n") ;
    }
    else { //add new node 
        new_node->file = malloc((strlen(file)+1)*sizeof(char)) ;
        strcpy(new_node->file,file) ;
        new_node->my_socket = arg_socket ;
        new_node->next_node = NULL ;
        if(queue->last_node==NULL) {  //queue is empty so add the first node
            queue->first_node = new_node ;
            queue->last_node = new_node ;
        }
        else {  //insert new node in the end
            queue->last_node->next_node = new_node ;
            queue->last_node = new_node ;
        }
    }
    printf("[Thread: %ld]:  Adding file %s to the queue\n",pthread_self(),new_node->file);
    pthread_mutex_unlock(&queue_type.mutex) ;  //unlock mutex in order to be used by another thread 
}

void remove_from_queue(my_queue* queue) {
    int error=0 ;
    pthread_mutex_lock(&queue_type.mutex) ; //lock mutex so that no other thread has access in the queue 
    while(queue->first_node==NULL) { //queue is empty
        pthread_cond_wait(&queue_type.cond_work,&queue_type.mutex) ;
    }
    printf("[Thread: %ld]:  Received task: <%s,%d>\n",pthread_self(),queue->first_node->file,queue->first_node->my_socket) ;
    //Write in socket
    pthread_mutex_lock(&write_socket_mutex) ;   //only one working thread writes in socket at a time
    if((write_in_socket(queue->first_node->file,queue->first_node->my_socket))==-1) {
        perror_fun("Failed writing in socket!\n",error) ;
    }
    pthread_mutex_unlock(&write_socket_mutex) ;

    //second node will become first node after the removal from the queue
    my_node* second_node = queue->first_node->next_node;  
    my_node* first_node = queue->first_node;
    queue->first_node = second_node ;
    free(first_node->file) ;
    free(first_node) ;
    queue_type.entries_in_queue-- ;
    if(queue->first_node==NULL) { //if last node was removed from queue
        queue->last_node = NULL ;
    }
    pthread_mutex_unlock(&queue_type.mutex) ; //now unlock mutex as the removal is ok
}

int write_in_socket(char* filename, int sockfd) {
    struct stat st ; //use struct stat to find the size of file called <filename> in bytes
    int error=0, nread;
    unsigned int size ;
    stat(filename,&st) ;
    size = st.st_size ; //total size of file
    int fd= open(filename, O_RDONLY) ; //open file
    if(fd==-1) {
        perror_fun("Opening file failed\n",error) ;
        return -1 ;
    }
    char name[queue_type.block_size], *file = malloc((size+1)*sizeof(char))  ;
    strcpy(name,filename) ;
    printf("[Thread: %ld]:  About to read %s\n",pthread_self(),filename) ;
    while((nread=read(fd,file,size))>0) { //read file
    }
    file[size] = '\0' ;
    //send name of file through socket
    if(send(sockfd,name,sizeof(name),0)==-1) {
        perror_fun("Failed writing this block of bytes in socket\n",error) ;
    }
    free(file) ; 
    return 0;
}

//function for error handling, using strerror because we cannot use perror() function because 
//the functions of the pthread library do not set the value of the variable errno (from theory of syspro class)
void perror_fun(char *str,int err) {
    fprintf(stderr,"%s:   %s\n",str,strerror(err)) ;
}