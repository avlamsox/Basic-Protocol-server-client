#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "gen.h"

#define SERVER_PORT 20211
#define SERVER_ADDR "127.0.0.1"

int main(int argc, char *argv[])
{
	int cli_sock, txd_bytes, rxd_bytes;
	char buffer[4096];
	struct sockaddr_in cli_addr;

	struct cmd_msg cli;
	//cli.cmd = GET_HOSTNAME;
	//cli.cmd = GET_TIME;
	//cli.cmd = GET_USER;
	cli.cmd = GET_OS;
	char *msg = "Hi Server";

	strncpy(cli.message, msg,sizeof(msg));

	printf("Command %d and Message %s\n",cli.cmd, cli.message);
	
	//Create client socket for communication
	cli_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(!cli_sock){
		fprintf(stderr, "Failed to create client socket\n");
		exit(1);
	}

	//setup client socket structure
	bzero(&cli_addr, sizeof(cli_addr));
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_port = htons(SERVER_PORT);
	
	if(!inet_aton(SERVER_ADDR, &cli_addr.sin_addr)){
		fprintf(stderr, "Failed to convert internet address\n");
		close(cli_sock);
		exit(1);
	}
	
	//Connect to the server
	if(connect(cli_sock, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) != 0){
		fprintf(stderr,"Failed to connect to server\n");
		close(cli_sock);
		exit(1);
	}
	
	//Send message to server
	if(txd_bytes = send(cli_sock, &cli, msg_len, 0)) {
		printf("Sent %d bytes to server\n",txd_bytes);
		//Now let's wait for server to send us back what we requested
		if(rxd_bytes = recv(cli_sock, buffer, 4096, 0)){
			printf("Received %d bytes from server\n",rxd_bytes);
			struct cmd_msg *cli_t = (struct cmd_msg *)&buffer;
			printf("We received our query response %s and command was %d\n",cli_t->message, cli_t->cmd);
		}
	}
	
	

	close(cli_sock);
	return 0;

}	
