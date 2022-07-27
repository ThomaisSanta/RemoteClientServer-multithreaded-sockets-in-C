#include "queue.h"
#include <sys/types.h>                         
#include <sys/socket.h> 
#include <sys/stat.h>                       
#include <netinet/in.h> 
#include <unistd.h>
#include <fcntl.h>              
#include <netdb.h>                       
#include <arpa/inet.h>

char directory_name[512] ;  //we assume that a directory name cannot have more that 511 characters

typedef struct client_struct {
    struct sockaddr_in client ;
    int my_socket ;
} client_struct;

typedef void * (*thread_fun) (void *) ;

void *comm_fun(void *) ;
void fun_in_comm_fun(struct sockaddr_in, int) ;
void *work_fun(void *) ;
void read_directory(char*,int,my_queue*) ;

