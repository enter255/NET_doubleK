#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>

#define BUFSIZE 100
#define SIZE 5

char bingo[5][5] = {0};
int pickNum=0, bingoTF=0;
int cho[25] = {0};
int fin = 0;

void error_handling(char *message);
void init_bingo();
void drawBoard();
int isBingo();


int main(int argc, char **argv)
{
	int sock;
	char message[BUFSIZE];
	int str_len;
	char arr[25];
	int a = -1, b, c = 0, i,j;
	struct sockaddr_in serv_addr;

	srand(time(NULL));
	
	/*DONT TOUCH*/
	if (argc != 3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
                error_handling("socket() error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error!");
	/*DONT TOUCH*/


	//client_2 connected
	read(sock, message, BUFSIZE);
	printf("client%s access\n", message);

	//init & print my bingo
	init_bingo();
	printf("\n[ BINGO START! ]\n\n");
	drawBoard();


	
	/*GAME START*/
	
	while(1) {
		sleep(2);

	
		/*client 1's turn*/

		read(sock, message, BUFSIZE); //client 1's result
		if (strcmp(message,"Bingo Finished") == 0){
			printf("YOU LOSE ㅠㅠ\n");
			break;
		}
		cho[++a] = message[0];
		printf("\nPlayer 1's choice : %d\n\n", cho[a]);


		//remove pickNum on bingo
		for (b = 0; b < 5; b++){
			for (c = 0; c < 5; c++){
				if (bingo[b][c] == cho[a]) {
					bingo[b][c] = 'X';
				}
			}
		}
		//print bingo
		drawBoard();

		//is it bingo?
		fin = 0;
		bingoTF = isBingo();
		if(fin == 3) {
			printf("BINGO FINISH! YOU WIN!\n");
			strcpy(message,"Bingo Finished");
			write(sock,message,BUFSIZE);
			break;
		}
		



		/*client 2's turn*/
	
		sleep(3);
		printf("input your number : ");
		scanf("%d",&pickNum);

		//check pickNum's overlap
		for(j=0; j<=a; j++) {
			here:
			if(cho[j] == pickNum) {
				printf("\nthat number is already taken.");
				printf("\ninput your number : ");
				scanf("%d",&pickNum);
				goto here;
			}
		}
		cho[++a] = pickNum;


		//remove pickNum on bingo
		for(b=0; b<5; b++) {
			for(c=0; c<5; c++) {
				if(bingo[b][c] == pickNum)
					bingo[b][c] = 'X';
			}
		}
		//print bingo
		drawBoard();

		//is it bingo?
		fin = 0;
		bingoTF = isBingo();
		if(fin == 3) { 
			printf("BINGO FINISH. YOU WIN!\n");
			strcpy(message, "Bingo Finished");
			write(sock, message, BUFSIZE);
			break;
		}
	}
	printf("%s win\n", message);
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
