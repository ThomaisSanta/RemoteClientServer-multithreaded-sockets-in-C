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

int main(int argc,char* argv[]) {
    int error=0 ;
    if(argc!=9) {
        perror_fun("Arguments from terminal must be 9",error) ;
    }
    int port=atoi(argv[2]), arg_thread_pool_size=atoi(argv[4]), arg_queue_size=atoi(argv[6]), arg_block_size=atoi(argv[8]) ;
    int first_socket, size_name ;
    struct sockaddr_in server ;
    client_struct *cl_st = malloc(sizeof(client_struct));
    struct sockaddr *server_ptr=(struct sockaddr *)&server;
    struct sockaddr *client_ptr=(struct sockaddr *)&(cl_st->client);
    socklen_t client_length ;
    queue_type.queue_size = arg_queue_size ; 
    queue_type.block_size = arg_block_size ; 
    queue_type.thread_pool_size = arg_thread_pool_size ; 
    printf("Server's parameteres are:\nport:  %d\nthread_pool_size:  %d\nqueue_size:  %d\nBlock_size:  %d\n",port,arg_thread_pool_size,arg_queue_size,arg_block_size) ;
    //create socket
    if ((first_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror_fun("Create socket failed",error) ;
    }
    server.sin_family = AF_INET;       // Internet domain 
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    // Bind socket to address
    if (bind(first_socket, server_ptr, sizeof(server)) < 0) {
        perror_fun("Bind socket failed",error) ;
    }
    printf("Server was successfully initialized...\n") ;
    // Listen for connections
    if (listen(first_socket, 5) < 0) {
        perror_fun("Listen socket failed",error) ;
    }
    printf("Listening for connections to port %d\n",port) ;
    //initialize mutexes and condiotion variables
    pthread_mutex_init(&queue_type.mutex,0) ;
    pthread_mutex_init(&write_socket_mutex,0) ;
    pthread_cond_init(&queue_type.cond_comm,0) ;
    pthread_cond_init(&queue_type.cond_work,0) ;
    pthread_t work_thr_id[queue_type.thread_pool_size] ;  //number of working threads that will be created
    //function that will be called by working thread (source code is inside server_functions.c file)
    thread_fun work_thread_fun = &work_fun;
    //create working threads before a client's request
    for(int i=0 ; i<queue_type.thread_pool_size;i++) {
        pthread_create(&work_thr_id[i],NULL,work_thread_fun,NULL) ;
        pthread_detach(work_thr_id[i]) ;
    }
    // while a client exists (program terminates after Ctrl C)
    while(1) {
        pthread_t comm_thr_id ;
        client_length = sizeof(cl_st->client) ;
        //Accept conection with this client
        if ((cl_st->my_socket = accept(first_socket, client_ptr, &client_length)) < 0) {
            perror_fun("Accept connection failed",error) ;
        }
        size_name = sizeof(cl_st->client.sin_addr.s_addr) ;
        struct hostent * client_name = gethostbyaddr(&(cl_st->client.sin_addr.s_addr),size_name,AF_INET) ;
        printf("Accepted connection from %s\n\n",client_name->h_name);  //show client's name and not IP
        //function that will be called by communication thread (source code is inside server_functions.c file)
        thread_fun comm_thread_fun = &comm_fun ;
        //for each client create a communication thread
        pthread_create(&comm_thr_id,NULL,comm_thread_fun,(client_struct *)cl_st) ;
        pthread_detach(comm_thr_id) ;
    }
    exit(0) ;
}

