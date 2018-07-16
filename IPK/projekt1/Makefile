server_name = ipk-server
client_name = ipk-client
gcc = gcc -std=c99
gcc-gnu = gcc -std=gnu99
 
all:host server
	
host: ipk-client.c help_function.c
	$(gcc) $(client_name).c help_function.c -o $(client_name)

server: ipk-server.c help_function.c  
	$(gcc-gnu) $(server_name).c help_function.c -o $(server_name)
