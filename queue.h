#pragma once
#include <pthread.h>

// queue created with single linked list as it is not known how large the queue will grow

// struct of each node inside queue
typedef struct my_node {
    char *file ;
    int my_socket ;
    struct my_node *next_node ; 
} my_node ;

//struct of the queue
typedef struct my_queue {
    my_node *first_node ;
    my_node *last_node ;
} my_queue ;

//struct containig queue and other information in order to be known from all threads
typedef struct queue_struct {
    my_queue queue ;
    int queue_size ;
    int block_size ;
    int entries_in_queue ;
    int thread_pool_size ;
    pthread_mutex_t mutex ;
    pthread_cond_t cond_comm ;
    pthread_cond_t cond_work ;
} queue_struct ;

queue_struct queue_type ;  //global variable of type queue_struct

pthread_mutex_t write_socket_mutex ; //mutex for writing inside socket

void insert_in_queue(char*,int,my_queue*) ;
void remove_from_queue(my_queue*) ;
int write_in_socket(char*, int) ;
void perror_fun(char *,int) ;
