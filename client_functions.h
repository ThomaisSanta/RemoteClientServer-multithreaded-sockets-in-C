#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>                         
#include <sys/socket.h> 
#include <sys/stat.h>                       
#include <netinet/in.h>  
#include <unistd.h>             
#include <netdb.h>                       
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>

void mkdir_fun(char *) ;
void perror_fun(char *,int) ;