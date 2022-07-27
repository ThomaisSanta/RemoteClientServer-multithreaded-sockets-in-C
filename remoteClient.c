#include "client_functions.h"

int main (int argc,char* argv[]){
    int error=0 ;
    if(argc!=7) {
        perror_fun("Arguments from terminal must be 7",error) ;
    }
    int port=atoi(argv[4]), my_socket ;
    char *directory = malloc(strlen(argv[6])*sizeof(char)) ;
    strcpy(directory,argv[6]) ;
    printf("Client's parameters are:\nserverIP: %s\nport: %d\ndirectory: %s\n",argv[2],port,directory) ; //argv[2] is ip_address
    struct sockaddr_in server ;
    struct sockaddr *server_ptr = (struct sockaddr*)&server ;
    struct hostent *server_ip = gethostbyname(argv[2]) ; 
    server.sin_family = AF_INET ;  //Internet address family for IPv4
    memcpy(&server.sin_addr, server_ip->h_addr, server_ip->h_length);
    server.sin_port = htons(port) ; //port of server
    if ((my_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    	perror_fun("Socket failed",error) ;
    }  
    //connect client with server  
    if (connect(my_socket, server_ptr, sizeof(server)) < 0){
        perror_fun("Connection to server failed",error) ;
    }
    printf("Connecting to %s on port %d\n", argv[2], port);
    //send from client to server through socket the name of the directory
    //that client wants to copy from server
    if(sendto(my_socket,directory,strlen(directory)+1,0,server_ptr,sizeof(server))<0) {
        perror_fun("Sending directory path to socket from client to server failed",error) ;
    }
    // now receive files from server
    char filename[512] ;
    char *new_file ;
    while(1) { //client terminates after receiving termination string from server
        //receive name of file from server 
        recv(my_socket,filename,sizeof(filename),0) ;
        if(strcmp(filename,"END")==0) { //client terminates
            break;
        }
        printf("Received: %s\n",filename);
        new_file = malloc(sizeof(filename)*sizeof(char)) ;
        strcpy(new_file,filename) ;
        mkdir_fun(new_file) ; //create the directory hierarchy to put file server sent to client
        free(new_file) ;
        memset(filename,0,sizeof(filename)*sizeof(char));
    }    
    close(my_socket) ;
    free(directory) ;
    exit(0) ;
}