#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>

#include "gen.h"

#define SERVER_PORT 20211

struct cmd_msg *parseMessages(struct cmd_msg *msg)
{
	int ret;
	time_t now;
	struct cmd_msg *loc = msg;
	char result_buf[32] = {0};

	//Here we will parse the received message and send appropriate reply message
	switch(loc->cmd) {
	
	case GET_HOSTNAME:
		printf("We received a request to our hostname\n");
		ret = gethostname(loc->message, 100);
		if(ret)
			fprintf(stderr, "Failed to get hostname\n");
		else {
			printf("The hostname of server is %s\n",loc->message);	
			snprintf(result_buf, strlen(loc->message), "%s", loc->message);
		}
		break;

	case GET_TIME:
		printf("We received a request to get our time\n");
		
		ret = time(&now);
		if(!ret)
			fprintf(stderr, "Failed to get hostname\n");
		else  {
			printf("The Current time is %s\n",ctime(&now));	
			snprintf(loc->message, 32,"%s",ctime(&now));
			}

		break;
	
	case GET_USER:
		printf("We received a request to get current user \n");
		uid_t uid = geteuid();
		struct passwd *pw = getpwuid(uid);
		if(!pw)
			fprintf(stderr, "Failed to get hostname\n");
		else {
			printf("The Current user=%s with uid=%d \n",pw->pw_name, uid);	
			snprintf(loc->message, 32,"%s",pw->pw_name);
		}

		break;

	case GET_OS:
		printf("We received a request to get our OS details\n");
		FILE *fp;
		char s;
		int count = 0;
		fp = fopen("/etc/issue.net", "r");
		while((s = fgetc(fp)) != EOF) {
		  loc->message[count] = s;
		  printf("%c",loc->message[count]);
		  count = count + 1;
   		}
		fclose(fp);
		break;
		
	default :
		printf("Undefined command\n");

	}
		
	return loc;

}

int main(int argc, char *argv[])
{
	int rxd_bytes;
	char *resp = "Connected";
	char buffer[4096];
	//struct cmd_msg serv;
	struct sockaddr_in serv_addr;
	//Create a socket
	int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(!serv_sock){
		fprintf(stderr, "Failed to create socket\n");
		return -ENOMEM;
	}


	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//serv_addr.sin_addr.s_addr = INADDR_ANY;

	//Bind the socket - i.e assign a name to the socket
	if(bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0){
		fprintf(stderr, "Failed to bind socket\n");
		close(serv_sock);
		return -ENOMEM;
	} 
	
	//Listen for incoming connections
	if((listen(serv_sock, 5)) != 0){
		fprintf(stderr, "Failed to listen on the bound socket address\n");
		close(serv_sock);
		exit(1);
	}

	//Wait and parse connections forever
	while(1)
	{
		//Create a clientfd for communications
		int client_sock;
		char *out_buf;
		struct sockaddr_in client_addr;
		int addrlen = sizeof(client_addr);
	
		
		//Accept the client connection and hook it with our local client fd
		client_sock = accept(serv_sock, (struct sockaddr *)&client_addr, &addrlen);
		printf("%s:%d Connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		//Receive message from client
		if((rxd_bytes = recv(client_sock, buffer, 4096, 0))){
			struct cmd_msg *serv = (struct cmd_msg *)&buffer;
			struct cmd_msg *tmp_msg;
			printf("Received %d bytes of data \n",rxd_bytes);
			printf("Data from client is -- Command %d\n and Message %s\n",serv->cmd,serv->message);

			//We need to parse the received data here, for that we need to assign locat struct addres
			if((tmp_msg = parseMessages(serv))){
				printf("Out Buff filled with %s\n",tmp_msg->message);
				printf("Now we need to send this back to client\n");
				
			}

			//Send back the requested data to client
			send(client_sock, tmp_msg, msg_len, 0);
		}
	
	}
		


	return 0;
}	
