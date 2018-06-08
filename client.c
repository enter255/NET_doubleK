#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>

#define BUFSIZE 100

void error_handling(char *message);

int main(int argc, char **argv)
{	
	int sock;
	char message[BUFSIZE];
	int str_len = 0;
	char arr[25], bingo[5][5];
	int a, b, c = 0;
	char cho[20] = { 0, }; //client가 선택할 숫자list
	struct sockaddr_in serv_addr;

	srand(time(NULL));

	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0); 
	if(sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));  
	serv_addr.sin_family=AF_INET;  
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);  
	serv_addr.sin_port=htons(atoi(argv[2])); 

	//request to be connected with server 
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)  
		error_handling("connect() error!");  
	//read client's name from server
	read(sock, message, BUFSIZE);
	a = atoi(message);
	printf("you are %dth accessed client.\n",a+1);

	for (a = 0; a < 25; a++) { //초기 빙고판 생성
		arr[a] = rand() % 25 + 1;
		for (b = 0; b < a; b++){
			if (arr[a] == arr[b]){
				--a;
				break;
			}
		}
	}
	for (a = 0; a < 5; a++){
		for (b = 0; b < 5; b++){
			bingo[a][b] = arr[a * 5 + b];
		}
	}


	//print bingo
	printf("\nBINGO START!\n\n");
	for (a = 0; a < 5; a++){
		for (b = 0; b < 5; b++){
			printf("%d\t", bingo[a][b]);
		}
		printf("\n");
	}


	/*게임 시작 - client1's turn is first*/
/*	for (a = 0; a < 20; a++){ //게임 시작. 최대 25번의 turn.
		sleep(2);

		printf("input your number : ");
		scanf("%d",&cho[(a * 2)]); //client1이 먼저 숫자 선택
		for (b = 0; b < (a * 2); b++){
			here:
			if (cho[(a * 2)] == cho[b]){ //선택한 숫자가 이미 지워진 숫자일 때
				printf("that number is already taken.\n");
				printf("input your number : ");
				scanf("%d", &cho[(a * 2)]);
				goto here;
			}
		}

		for (b = 0; b < 5; b++){
			for (c = 0; c < 5; c++){
				if (bingo[b][c] == cho[(a * 2)]){
					bingo[b][c] = 'X';
				}
			}
		}

		for (b = 0; b < 5; b++){
			for (c = 0; c < 5; c++){
			if (bingo[b][c] == 'X')
					printf("%c\t", bingo[b][c]);
				else
					printf("%d\t", bingo[b][c]);
			}printf("\n");
		}

		int fin = 0;//빙고 확인
		int garo = 0;
		for (b = 0; b < 5; b++){
			garo = 0;
			for (c = 1; c < 5; c++){
				if (bingo[b][0] == bingo[b][c]){
					garo++;
					if (garo == 4){
						fin++;
					}
				}
			}
		}
		int sero = 0;
		for (b = 0; b < 5; b++){
			sero = 0;
			for (c = 1; c < 5; c++){
				if (bingo[0][b] == bingo[c][b]){
					sero++;
					if (sero == 4){
						fin++;
					}
				}
			}
		}
		int cross1 = 0;
		for (b = 1; b < 5; b++){
			if (bingo[0][0] == bingo[b][b]){
				cross1++;
				if (cross1 == 4){
					fin++;
				}
			}
		}
		int cross2 = 0;
		for (b = 1; b < 5; b++){
			if (bingo[0][4] == bingo[b][4 - b]){
				cross2++;
				if (cross2 == 4){
					fin++;
				}
			}
		}
		if (fin == 5){
			printf("BINGO FINISH!\n  YOU WIN!");
			message[0]=99;
			write(sock, message, BUFSIZE);
			break;
		}
		message[0] = cho[(a * 2)];
*/

		/*send client1's choice to server*/
//		write(sock, message, BUFSIZE); 

		/*receive client2's choice from server*/
/*		read(sock, message, BUFSIZE);	
		if (message[0] == 99){
			printf("YOU LOSE TT");
			break;
		}
		sleep(3);

		cho[(a * 2) + 1] = message[0];
		printf("\nclient2's choice : %d\n\n", cho[(a * 2) + 1]);

		//print bingo
		for (b = 0; b < 5; b++){
			for (c = 0; c < 5; c++){
				if (bingo[b][c] == cho[(a * 2) + 1]){
					bingo[b][c] = 'X';
				}
			}
		}

		for (b = 0; b < 5; b++){
			for (c = 0; c < 5; c++){
				if (bingo[b][c] == 'X')
					printf("%c\t", bingo[b][c]);
				else
					printf("%d\t", bingo[b][c]);
			}printf("\n");
		}
		fin = 0;
		garo = 0;
		for (b = 0; b < 5; b++){
			garo = 0;
			for (c = 1; c < 5; c++){
				if (bingo[b][0] == bingo[b][c]){
					garo++;
					if (garo == 4){
						fin++;
					}
				}
			}
		}
		sero = 0;
		for (b = 0; b < 5; b++){
			sero = 0;
			for (c = 1; c < 5; c++){
				if (bingo[0][b] == bingo[c][b]){
					sero++;
					if (sero == 4){
						fin++;
					}
				}
			}
		}
		cross1 = 0;
		for (b = 1; b < 5; b++){
			if (bingo[0][0] == bingo[b][b]){
				cross1++;
				if (cross1 == 4){
					fin++;
				}
			}
		}
		cross2 = 0;
		for (b = 1; b < 5; b++){
			if (bingo[0][4] == bingo[b][4 - b]){
				cross2++;
				if (cross2 == 4){
					fin++;
				}
			}
		}
		if (fin == 5){
			printf("BINGO FINISH!\n  YOU WIN!");
			message[0] = 99;
			write(sock, message, BUFSIZE);
			break;
		} 
	}
*/
	close(sock);  
	return 0;  
}  
   
void error_handling(char *message)  
{  
	fputs(message, stderr);  
	fputc('\n', stderr);  
	exit(1);  
}
