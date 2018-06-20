#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>

#define BUFSIZE 20
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

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)  
		error_handling("connect() error!");  

	printf("you are 1st Player.\n");
	/*DONT TOUCH*/	

	
	//init & print my bingo
	init_bingo();
	printf("\n[ BINGO START! ]\n\n");
	drawBoard();


	/*GAME START*/

	while(1) {
		sleep(2);
		//memset(&message ,0, sizeof(message));

		/*client 1's turn*/

		printf("input your number : ");
                scanf("%d",&pickNum); //pick up number

		//check pickNum's overlap
		for (j = 0; j <= a; j++){
			here:
			if (cho[j] == pickNum){
				printf("\nthat number is already taken.");
				printf("\ninput your number : ");
				scanf("%d", &pickNum);
				goto here;
			}
		}
		cho[++a] = pickNum;

		//remove pickNum on bingo
		for (b = 0; b < 5; b++){
			for (c = 0; c < 5; c++){
				if (bingo[b][c] == pickNum){
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
			printf("BINGO FINISH!  YOU WIN!\n");
			strcpy(message,"Bingo Finished");
                        write(sock, message, BUFSIZE);
			break;
		}
		//write client1's pick num to server
		sprintf(message,"%d",pickNum);
		message[strlen(message)] = '\0';
		//message[0] = pickNum;
		int n = write(sock, message, BUFSIZE);  
		printf("** cli_1 write '%s' to server.\n",message);
		printf("n : %d\n",n);

		/*client2's turn*/

		//read client2's result from server
		read(sock, message, BUFSIZE);
		if (strcmp(message,"Bingo Finished") == 0){
			printf("YOU LOSE ㅠㅠ\n");
			break;
		}
		sleep(3);

		printf("** cli_2's choice : %s\n",message);
		//cho[++a] = atoi(message) + '0';
		sscanf(message,"%d",&cho[++a]);
		printf("Player 2's choice : %d\n",cho[a]);

		//remove pickNum on bingo
                for (b = 0; b < 5; b++){
                        for (c = 0; c < 5; c++){
                                if (bingo[b][c] == cho[a]){
                                        bingo[b][c] = 'X';
                                }
                        }
                }

		//print bingo
		drawBoard();
		
		//is bingo?
		fin = 0;
		bingoTF = isBingo();
		if (fin == 3){ //it is bingo!
			printf("BINGO FINISH. YOU WIN!\n");
			strcpy(message, "Bingo Finished");
			write(sock, message, BUFSIZE);
			break;
		}
		
	}
	close(sock);  
	return 0;  
}  
   
void error_handling(char *message)  
{  
	fputs(message, stderr);  
	fputc('\n', stderr);  
	exit(1);  
}

void init_bingo() {
        int i,j, arr[25] = {0,};

        srand(time(NULL));
        for (i = 0; i < 25; i++) { //초기 빙고판 생성
                arr[i] = rand() % 25 + 1;
                for (j = 0; j < i; j++){ //중복확인
                        if (arr[i] == arr[j]){
                                --i;
                                break;
                        }
                }
        }
        for (i = 0; i < 5; i++){
                for (j = 0; j < 5; j++)
                        bingo[i][j] = arr[i * 5 + j];
        }
}


int isBingo () {
        int row=0, col=0;
        int dia1=0, dia2=0;
	int i,j,k;

        for (i = 0; i < 5; i++){
                row = 0;
                for (j = 1; j < 5; j++){
                        if (bingo[i][0] == bingo[i][j]){
                                row++;
                                if (row == 4)
                                        fin++;
                        }
                }
        }
        for (i = 0; i < 5; i++){
                col = 0;
                for (j = 1; j < 5; j++){
                        if (bingo[0][i] == bingo[j][i]){
                                col++;
                                if (col == 4)
                                        fin++;
                        }
                }
        }
        for (i = 1; i < 5; i++){
                if (bingo[0][0] == bingo[i][i]){
                        dia1++;
                        if (dia1 == 4)
                                fin++;
                }
        }
        for (i = 1; i < 5; i++){
                if (bingo[0][4] == bingo[i][4-i]){
                        dia2++;
                        if (dia2 == 4)
                                fin++;
                }
        }
}

void drawBoard () {
        int x=0, y=0;
        char reset[] = "\033[m";

        for (y=0; y<SIZE; y++) {
		//upside
                for (x=0; x<SIZE; x++) {
                        if(bingo[y][x] == 'X')
                                printf("\033[48;2;225;141;154m");
                        else
                                printf("\033[48;2;133;184;248m");
                        printf("%*s", 8, "");
                        printf("%s",reset);
                }
                printf("\n");

		//middle side with letters
                for (x=0; x<SIZE; x++) {
                        if(bingo[y][x] == 'X'){
                                printf("\033[48;2;225;141;154m");//pink로 해줘
                                printf("%*s%2c%*s", 3, "", bingo[y][x], 3, "");
                        }
                        else{
                                printf("\033[48;2;133;184;248m");//blue로 해줘
                                printf("%*s%2d%*s", 3, "", bingo[y][x], 3, "");
                        }
                        printf("%s",reset);
                }
                printf("\n");

		//downside
                for (x=0; x<SIZE; x++) {
                        if(bingo[y][x] == 'X')
                                printf("\033[48;2;225;141;154m");
                        else
                                printf("\033[48;2;133;184;248m");
                        printf("%*s", 8, "");
                        printf("%s",reset);
                }
                printf("\n");
        }
}

