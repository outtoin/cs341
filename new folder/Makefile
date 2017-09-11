all : client server
	gcc -c -o client client.c
	gcc -c -o server server.c

client : newclient.c
	gcc -c -o client client.c

server : newserver.c
	gcc -c -o server server.c

clean :
	rm client
	rm server
