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

//변수
char bingo[5][5] = {0};
int pickNum=0, winner=0, bingoTF=0, k=-1;
int cho[25] = {0};

//함수
void error_handling(char *message);
void init_bingo();
//void print_bingo();
void drawBoard();
int isBingo();

int main(int argc, char **argv)
{	
	int n=0;
	int sock;
	char message[BUFSIZE];
	int str_len = 0;
	char arr[25];
	int i,j;
	int a, b, c = 0;
	struct sockaddr_in serv_addr;
	int myIndex = 0, maxPlayer = 0;

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
 
	//read client's index and total # of clients from server
	read(sock, message, BUFSIZE);
	sscanf(message, "%d %d", &myIndex, &maxPlayer);
	printf("you are %dth accessed client.\n",myIndex);

	//init & print my bingo
	init_bingo();
	printf("\nBINGO START!\n\n");
	//print_bingo();
	drawBoard();


	/*게임 시작*/
	i=1;
	while(1) {
		if(i > maxPlayer)
			i=1;
		//sleep(2);

		if(i!=myIndex) {
			printf("here_1\n"); sleep(5);
			read(sock, message, BUFSIZE);
			sscanf(message, "%d %d", &pickNum, &bingoTF);
			 printf("pickNum : %d  bingoTF : %d\n",pickNum,bingoTF);
			if(bingoTF == 1) {
				printf("%dth client Win. YOU LOSE.\n",i);
				break;
			}
		}
		else if(i == myIndex) {
			printf("input your number : ");
			scanf("%d",&pickNum); //숫자 선택

			//선택한 숫자 중복여부 체크
			for (j = 0; j <= k; j++){
				here:
				if (cho[j] == pickNum){ 
					printf("\nthat number is already taken.\n");
					printf("input your number : ");
					scanf("%d", &pickNum);
					goto here;
				}
			}

			cho[++k] = pickNum;
		}


		//선택한 숫자 지우기
		for (b = 0; b < 5; b++){
			for (c = 0; c < 5; c++){
				if (bingo[b][c] == pickNum){
					bingo[b][c] = 'X';
				}
			}
		}
		//print_bingo();
		drawBoard();

		//빙고 확인
		bingoTF = isBingo();	

		/*send my bingo result to server*/
		sprintf(message, "%d %d", pickNum, bingoTF);
		n = write(sock, message, BUFSIZE); 
		printf("n : %d\n",n);
		/*receive total bingo result from server*/
		read(sock, message, BUFSIZE);
		sscanf(message, "%d %d %d", &winner, &pickNum, &bingoTF);	
		if(winner != -1) {
			printf("%dth client win. YOU LOSE.\n",winner);
			break;
		}

		i++;
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
	int a,b, arr[25] = {0,};

	srand(time(NULL));
	for (a = 0; a < 25; a++) { //초기 빙고판 생성
		arr[a] = rand() % 25 + 1;
		for (b = 0; b < a; b++){ //중복확인
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
}

/*
void print_bingo() {
	int a, b;

	for (a = 0; a < 5; a++){
		for (b = 0; b < 5; b++){
			if(bingo[a][b] == 'X')
				printf("%c\t", bingo[a][b]);
			else
				printf("%d\t", bingo[a][b]-'0');
		}
		printf("\n");
	}
}
*/

void drawBoard () {
        int x=0, y=0;
	char reset[] = "\033[m";

        for (y=0;y<SIZE;y++)
        {
                for (x=0;x<SIZE;x++) {
                        if(bingo[y][x] == 'X'){
                                printf("\033[48;2;225;141;154m");
                        }
                        else{
                                printf("\033[48;2;133;184;248m");
                        }
                        printf("%*s", 8, "");
                        printf("%s",reset);
                }
                printf("\n");

                for (x=0;x<SIZE;x++) {
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

                for (x=0;x<SIZE;x++) {
                        if(bingo[y][x] == 'X'){
                                printf("\033[48;2;225;141;154m");
                        }
                        else{
                                printf("\033[48;2;133;184;248m");
                        }
                        printf("%*s", 8, "");
                        printf("%s",reset);
                }
                printf("\n");
        }
}
                               

int isBingo () {
	int fin=0, garo=0, sero=0;
	int a,b,c;
	int cross1=0, cross2=0;

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
	for (b = 1; b < 5; b++){
		if (bingo[0][0] == bingo[b][b]){
			cross1++;
			if (cross1 == 4){
				fin++;
			}
		}
	}
	for (b = 1; b < 5; b++){
		if (bingo[0][4] == bingo[b][4 - b]){
			cross2++;
			if (cross2 == 4){
				fin++;
			}
		}
	}
	

	if (fin == 3)
		return 1;
	else
		return 0;
	
}
