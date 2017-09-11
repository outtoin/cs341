client : newclient.c
	gcc -c -o newclient newclient.c

server : newserver.c
	gcc -c -o newserver newserver.c

clean :
	rm newclient
	rm newserver
