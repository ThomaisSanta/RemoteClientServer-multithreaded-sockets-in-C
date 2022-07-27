#include "client_functions.h"

//function for making the same directory hierarchy as the directory 
// <file> (the directory client asked for)
void mkdir_fun(char *file) {
    char *file_temp = malloc(strlen(file)*sizeof(char)) ;
    strcpy(file_temp,file) ; 
    char *path1 = malloc(512*sizeof(char)) ; //we assume that every path of a file has not more than 511 characters
    memset(path1,0,512) ; //set path1 with zeros
    int count=0 ;
    char rec[22] = "./received_directories" ; //default directory in which a client will save the directory that server sent
    int fd ;
    char *parent_dir = malloc(512*sizeof(char)) ;
    memset(parent_dir,0,512) ;
    char filename[40] ;
    strcat(path1,rec) ;
    strcat(path1,"/") ;
    struct stat st ;
    //split argument file with limit "/" in order to know how many words path has
    char *token1 = strtok(file,"/") , *token ;
    while (token1 != NULL) {
        count++ ;                  //count of path's words 
        token1 = strtok(NULL, "/");
    }
    //again split file_temp variable now to find every word of path
    token = strtok(file_temp,"/");
    strcat(parent_dir,"../") ;   //in order to know how far is from the first directory
    while(token!=NULL) {
        strcat(path1,token) ;
        strcat(parent_dir,"../") ;
        if(stat(path1,&st)==-1) {  //path1 is not a file so it is directory
            mkdir(path1,0777) ;    // now make this directory
        }
        --count ;
        strcat(path1,"/") ;
        token = strtok(NULL,"/");
        if(count==1) {   // last word of argument file
            strcpy(filename,token) ;  //last word is the name of the file which will be created
            break ;  // end loop
        }
    }
    chdir(path1) ;  // change to the directory that file needs to be created
    if((fd=creat(filename,0644))==-1) { //create file
        perror("failed to create file\n") ;
    }
    chdir(parent_dir) ; // go to the first directory in order to create the next path if function is called again
    free(file_temp) ;
    free(parent_dir) ;
    free(path1) ;
}

//function for error handling
void perror_fun(char *str,int err) {
    fprintf(stderr,"%s:   %s\n",str,strerror(err)) ;
}
