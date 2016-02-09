all:hserver rclient
hserver:server.c
	gcc server.c -o hserver
rclient:client.c
	gcc client.c -o rclient
clean:
	rm hserver rclient
