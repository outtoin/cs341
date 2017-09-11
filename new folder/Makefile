all : client server
	gcc -c -o client client.c
	gcc -c -o server server.c

client : client.c
	gcc -c -o client client.c

server : server.c
	gcc -c -o server server.c

clean :
	rm client
	rm server
