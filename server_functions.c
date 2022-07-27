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
#include "server_functions.h"

//function that is called by communication thread
void * comm_fun(void *args) {
    client_struct* cl_st = (client_struct *) args ;
    //call the function below with arguments the client and its socket
    fun_in_comm_fun(cl_st->client, cl_st->my_socket) ;
}

void fun_in_comm_fun(struct sockaddr_in client, int my_socket) {
    struct sockaddr *client_ptr=(struct sockaddr *)&(client);
    socklen_t client_length = sizeof(client) ;
    int count, error=0 ;
    char termination_str[4] = "END" ; //when server has no more files to send to the client then server
    //sends the above termination string and client can then terminate

    //server receives from client through the socket the name of the directory that client asked for
    if((count=recvfrom(my_socket,directory_name,sizeof(directory_name),0,client_ptr,&client_length))<0) {
        perror_fun("Server failed to receive from client",error) ;
    } 
    directory_name[sizeof(directory_name)-1]='\0'; // force string termination 
    char* dir_name = malloc(strlen(directory_name)*sizeof(char)) ;
    strcpy(dir_name,directory_name) ;
    printf("[Thread: %ld]:  About to scan directory %s\n",pthread_self(),dir_name) ;
    //communication thread reads recursively the directory and adds names of files in the queue
    read_directory(dir_name,my_socket,&(queue_type.queue)) ;
    usleep(20000) ; //so that all files can be sent in the right way
    //protect socket so that only one thread writes in it at the time
    pthread_mutex_lock(&write_socket_mutex) ;
    //server will now send termination string through socket because all files have been sent to the client
    //and client will terminate the session with server
    if(send(my_socket,termination_str,sizeof(termination_str),0)==-1) {
        perror_fun("Failed writing this block of bytes in socket\n",error) ;
    }        
    pthread_mutex_unlock(&write_socket_mutex) ;
    free(dir_name) ;
}

//function that is called by the worker threads
void *work_fun(void *args) {
    //while number of worker threads is greater than 0 OR queue is not empty
    while(queue_type.thread_pool_size>0 || queue_type.entries_in_queue>0) {
        //remove an item from queue
        remove_from_queue(&queue_type.queue) ;
        //now another thread can have access to the queue
        pthread_cond_signal(&queue_type.cond_comm) ;
    }
}

//function to extract all name of files recursively from a directory
void read_directory(char* name,int socket, my_queue* queue) {
    struct dirent *contents;
    DIR *dir = opendir(name) , *temp ;
    char path[queue_type.block_size] ;
    if (dir == NULL){
      printf("Directory cannot be opened!\n");
    }
    while ((contents = readdir(dir)) != NULL) {
      if(strcmp(contents->d_name,".") && strcmp(contents->d_name,"..")) { //don't need current and parent directories
        strcpy(path,name) ;
        strcat(path,"/");
        strcat(path,contents->d_name);
        temp = opendir(path) ; //check if <path> is a directory
        if(temp==NULL) {  //not directory, so it is a file
          insert_in_queue(path,socket,queue) ; //insert name of file in queue
          pthread_cond_signal(&queue_type.cond_work) ; //now another thread can use the queue
        }
        else { //it is a directory so call again this function with argument the current direcotry
          read_directory(path,socket,queue) ;
        }
      }
    }
    closedir(dir);
}
