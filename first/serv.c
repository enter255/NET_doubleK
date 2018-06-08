#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>

#define BUFSIZE 1024
#define MAX_PLAYER 3

void error_handling(char* message); 

int main (int argc, char **argv) 
{
	int serv_sock;
	int clnt_sock;
	char buf[BUFSIZE];
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	struct timeval timeout;
	fd_set reads, cpyReads;
	socklen_t addrSz;
	int fdMax, strLen, fdNum, i;

	if(argc != 2) {
		printf("Usage : %s <PORT>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET,SOCK_STREAM,0);
	if(serv_sock == -1){
		error_handling("socket() error");
	}

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
		error_handling("bind() error");
	}

	printf("##before listening\n");

	if(listen(serv_sock,5) == -1){
		error_handling("listen() error");
	}

	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fdMax = serv_sock;

	printf("##before go to while\n");
	while(1) 
	{
		cpyReads = reads;
		timeout.tv_sec = 10;	// 10 sec for timeout -> player's thinking time is max 10 sec
		timeout.tv_usec = 10000;

		printf("##before start select\n");

		fdNum = select(fdMax + 1, &cpyReads, 0, 0, &timeout);
		if(fdNum == -1){
			break;
		}
		else if(fdNum == 0)
			continue;

		printf("##before accept start\n");

		for(i = 0; i < MAX_PLAYER; i++){
			if(FD_ISSET(serv_sock, &cpyReads)){
				clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &addrSz);
				if(clnt_sock == -1)
					error_handling("accept() error");


				FD_SET(clnt_sock, &reads);
				if(fdMax < clnt_sock)
					fdMax = clnt_sock;
				// tell each client that you are ith accepted client
				sprintf(buf, "%d", i);
				printf("client connected: %d\n", clnt_sock);
				write(clnt_sock, buf, sizeof(buf));

			}
		}
		/*
		for(i = 0; i < fdMax+1; i++) 
		{
			if(FD_ISSET(i,&cpyReads)) 
			{
				if(i == serv_sock)	// SERVER: process for accept client
				{

					addrSz = sizeof(clnt_addr);
					clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addrSz);
					FD_SET(clnt_sock, &reads);

					if(fdMax < clnt_sock)
						fdMax = clnt_sock;

					printf("connected client : %d\n", clnt_sock);
				}
				else	// CLIENT: process for communication with client
				{
				
					 strLen = read(i,buf, BUFSIZE);
					
					if(strLen == 0) {
						FD_CLR(i,&reads);
						close(i);
						printf("close client : %d\n", i);               
					}
					else{
						write(i,buf,strLen);
					}
					

				}
			}
		}
		*/
	}

	close(serv_sock);

	return 0;
}

void error_handling(char* message) {
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
