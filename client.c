//2016115113 김선희

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#define BUF_SIZE 30

void error_handling(char *message);

int main (int argc, char *argv[]) {
	int sock;
	struct sockaddr_in serv_addr;
	char message[BUF_SIZE];
	int str_len=0;

	if(argc != 3) { //input form is not correct
		printf("Usage : %s <IP> <port>\n",argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM,0); //make socket
	if(sock == -1)
		error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr)); //let server address
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	//request to be connected with server
	if(connect(sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) 
		error_handling("connect() error!");

	while(1) {
		fputs("Input message(Q to quit) : ",stdout);
		strcpy(message," ");
		fgets(message, BUF_SIZE, stdin);
		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
			break;
		
		str_len = strlen(message);
		write(sock,message,str_len); //write to server
		if( read(sock,message,BUF_SIZE-1) == -1 ) //read from server
			error_handling("read() error!");
		printf("Message from server : %s\n",message); //print message
	}
	close(sock);
	return 0;
}

void error_handling(char *message) {
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}


