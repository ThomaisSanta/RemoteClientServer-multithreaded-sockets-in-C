
CFLAGS=-g

all : remoteClient dataServer

program1=remoteClient
program2=dataServer 


objects1=client_functions.o remoteClient.o
objects2=dataServer.o server_functions.o queue.o

$(program1) : $(objects1)
	gcc $(objects1) -o $(program1) 

$(program2) : $(objects2)
	gcc $(objects2) -o $(program2) -lpthread

dataServer.o : server_functions.h queue.h
server_functions.o : server_functions.h queue.h 
queue.o : queue.h

client_functions.o : client_functions.h
remoteClient.o : client_functions.h

clean:
	rm -f dataServer \
	remoteClient