#include "tetris.h"
#define MAX_RANK 100 // 순위의 최대 개수
#define NAME_LENGTH 50 // 이름의 최대 길이


// 함수 선언 추가
void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate);

static struct sigaction act, oact;

typedef struct {
	char name[NAME_LENGTH];
	int score;
} Rank;

Rank rankList[MAX_RANK] = {0};

int rankCount = 0; // 순위 개수 저장
int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	createRankList(); // 순위 데이터 초기화

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7; // 두 번째 next block
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);

	/* 두 번째 next block을 보여주는 공간의 태두리를 그린다.*/
	move(9, WIDTH + 10);
	printw("NEXT BLOCK 2");
	DrawBox(10, WIDTH + 10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(16,WIDTH+10);
	printw("SCORE");
	DrawBox(17,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}

	// 두 번째 next block
	for (i=0; i < 4; i++) {
		move(11 + i, WIDTH + 13);
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[2]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			} else {
				printw(" ");
			}
		}	
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}
	}
	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	char name[NAME_LENGTH];
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			getch();
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();
			newRank(score); // 이름과 점수 저장
			return;
		}

		// 블럭과 그림자 그리기
		DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	} while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score); // 이름과 점수 저장


}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	char choice = wgetch(stdscr);

	switch (choice) {
		case '2': // 순위 보기
			rank();
			break;
	}
	return choice;
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;

	for(i = 0; i < BLOCK_HEIGHT; i++) {
		for(j = 0; j < BLOCK_WIDTH; j++) {
			if(block[currentBlock][blockRotate][i][j] == 1) {
				if(blockY + i < 0 || blockX + j < 0 || blockX + j >= WIDTH || blockY + i >= HEIGHT || f[blockY + i][blockX + j] == 1)
					return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	int prevRotate = blockRotate;
	int prevY = blockY;
	int prevX = blockX;

	// 키에 따라 블럭의 위치를 바꿔준다.
	switch(command) {
		case KEY_UP:
			prevRotate = (blockRotate + 3) % 4; // 회전하기
			break;
		case KEY_DOWN:
			prevY = blockY - 1; // 아래로 이동하기
			break;
		case KEY_RIGHT:
			prevX = blockX - 1; // 왼쪽으로 이동하기
			break;
		case KEY_LEFT:
			prevX = blockX + 1; // 오른쪽으로 이동하기
			break;
		default:
			break;
	}

	// 이전 블럭 지우기
	DrawBlock(prevY, prevX, currentBlock, prevRotate, '.');

	// 새로운 블럭 그리기
	DrawBlock(blockY, blockX, currentBlock, blockRotate, ' ');

	// 화면 갱신
	refresh();
}

void BlockDown(int sig){
	// 강의자료 p26-27의 플로우차트를 참고한다.
	//1. blockY를 1증가 시킨다.
	blockY++;
	//2. CheckToMove 함수를 호출하여, 블럭이 더이상 내려갈 수 있는지 확인한다.
	//   AddBlockToField 함수를 호출하여, 블럭을 필드에 추가시킨다.
	//   그리고 DeleteLine 함수를 호출하여, 꽉찬 구간이 있으면 지운다.
	//   그리고, score를 1증가 시킨다.
	//   그리고, next block을 준비시킨다.
	if(CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX)==0){
		blockY--;

		// 필드에 블록 추가 및 닿은 면적 계산
		int contactArea = AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
		
		// 삭제된 라인의 개수를 가져옵니다.
		int linesCleared = DeleteLine(field);

		// 점수 증가
		score += linesCleared * linesCleared * 100; // 삭제된 라인의 개수의 제곱에 비례하여 점수 증가
		score += contactArea * 10; // 닿은 면적에 비례하여 점수 증가


		// 게임 오버 조건 확인
		if (blockY < 0) {
			gameOver = 1;
		} else {
			// 다음 블럭 준비
			nextBlock[0] = nextBlock[1];
			nextBlock[1] = nextBlock[2];
			nextBlock[2] = rand() % 7;
			blockRotate = 0;
			blockY = -1;
			blockX = WIDTH / 2 - 2;
			DrawNextBlock(nextBlock);
		}
	}

	// 필드와 점수를 다시 그립니다.
	DrawField();
	PrintScore(score);
	refresh();

	//3. timed_out를 0으로 설정하여, 다음 타이머가 작동하도록 한다.
	timed_out=0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;
	int contactArea = 0; // 접촉 영역을 카운트하기 위한 변수
	// 블럭을 필드에 추가
	for(i = 0; i < BLOCK_HEIGHT; i++) {
		for(j = 0; j < BLOCK_WIDTH; j++) {
			if(block[currentBlock][blockRotate][i][j] == 1) {
				if(blockY + i >= 0 && blockY + i < HEIGHT && blockX + j >= 0 && blockX + j < WIDTH && f[blockY + i][blockX + j] == 0) {
					f[blockY + i][blockX + j] = 1;

					// 접촉 영역을 카운트 : 아래쪽이 필드의 경계에 닿는 경우
					if (blockY + i + 1 >= HEIGHT) {
						contactArea++;
					}
				}
			}
		}
	}

	return contactArea; // 접촉 영역의 개수를 반환
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int i, j, line = 0;

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	for (i = 0; i < HEIGHT; i++) {
		int flag = 1;
		for (j = 0; j < WIDTH; j++) {
			if (f[i][j] == 0) {
				flag = 0;
				break;
			}
		}
		//2. 꽉 찬 구간이 있으면, line을 1증가 시킨다.
		if (flag) {
			line++;
			for (j = i; j > 0 && j < HEIGHT; j--) {
				memcpy(f[j], f[j - 1], sizeof(f[0]));
			}
			if(i < HEIGHT){
				memset(f[0], 0, sizeof(f[0]));
			}
		}
	}

	return line;
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	int shadowY = y;

	// 블럭이 더 이상 내려갈 수 없는 위치를 계산
	while (CheckToMove(field, blockID, blockRotate, shadowY + 1, x)) {
		shadowY++;
	}

	// 그림자를 '/'로 표시
	DrawBlock(shadowY, x, blockID, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
	// 그림자 그리기
	DrawShadow(y, x, blockID, blockRotate);

	// 블럭 그리기
	DrawBlock(y, x, blockID, blockRotate, ' ');
}

void createRankList(){
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	FILE *fp;
	int totalRanks;
	char line[100];
	int rank = 0;
	
	//1. 파일 열기
	fp = fopen("rank.txt", "r");
	if (fp == NULL) {
		return;
	}

	//2. 파일에서 랭킹정보 읽어오기
	fscanf(fp, "%d", &totalRanks);
	while (fgets(line, sizeof(line), fp) && rank < totalRanks && rank < MAX_RANK) {
		sscanf(line, "%s %d", rankList[rank].name, &rankList[rank].score);
		rank++;
	}

	//4. 파일 닫기
	fclose(fp);
}

void rank(){
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화
	int X=1, Y=rankCount, ch, i, j;
	clear();

	//2. printw()로 3개의 메뉴출력
	printw("1. List ranks\n");
	printw("2. Search by name\n");
	printw("3. Delete rank\n");
	printw("4. Exit\n");

	//3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);

	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (ch == '1') {
		printw("Enter X and Y: ");
		scanw("%d %d", &X, &Y);

		// 순위 출력
		if (X <= Y && X >= 1 && Y <= rankCount) {
			for (i = X - 1; i < Y; i++) {
				printw("%d. %s %d\n", i + 1, rankList[i].name, rankList[i].score);
			}
		} else {
			printw("Invalid range.\n");
		}
	}

	//4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if ( ch == '2') {
		char str[NAMELEN+1];
		printw("Enter name: ");
		scanw("%s", str);

		int found = 0;
		for (i = 0; i < rankCount; i++) {
			if (strcmp(rankList[i].name, str) == 0) {
				printw("%d. %s %d\n", i + 1, rankList[i].name, rankList[i].score);
				found = 1;
			}
		}
		if (!found) {
			printw("Name not found.\n");
		}
	}

	//4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if ( ch == '3') {
		int num;
		printw("Enter rank number to delete: ");
		scanw("%d", &num);

		if (num >= 1 && num <= rankCount) {
			for (i = num -1; i < rankCount - 1; i++) {
				rankList[i] = rankList[i + 1];
			}
			rankCount--;
			printw("Rank delete.\n");
		} else {
			printw("Invalid rnak number. \n");
		}
	}
	getch();

}

void writeRankFile(){
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	FILE *fp = fopen("rank.txt", "w");
	if (fp == NULL) return;

	fprintf(fp, "%d\n", rankCount);
	for (int i = 0; i < rankCount; i++) {
		fprintf(fp, "%s %d\n", rankList[i].name, rankList[i].score);
	}

	fclose(fp);
}

void newRank(int score){
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char str[NAMELEN+1];
	int i;
	clear();
	//1. 사용자 이름을 입력받음
	printw("Enter your name: ");
	scanw("%19s", str);

	//2. 새로운 노드를 생성해 이름과 점수를 저장
	for (i = rankCount; i > 0; i--) {
		if (score > rankList[i-1].score) {
			rankList[i] = rankList[i-1];
		} else {
			break;
		}
	}
	rankList[i].score = score;
	strncpy(rankList[i].name, str, NAMELEN);

	rankCount++;
	writeRankFile();
}


void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
