#include <stdio.h>
#include <conio.h>							//_kbhit()
#include <time.h>							//srand(time(NULL))
#define TABLE_WIDTH			31
#define TABLE_HEIGHT		15
#define SCREEN_POSITION		5			//수평자세
#define STANDARD_DELAY		1000			//DELAY
#define LENGTH				1				//라켓 길이
#define BORDER				1				//가장자리
#define SPACE				0				//공간
#define RACKET_1			2				//플레이어 (1)
#define RACKET_2			3				//컴퓨터/플레이어2
#define BALL				4				//공
#define UP_RIGHT			1				//방향
#define DOWN_RIGHT			2				//	//
#define UP_LEFT				3				//	//
#define DOWN_LEFT			4				//	//
#define RIGHT				5				//	//
#define LEFT				6				//	//
#define SPECIAL_UP_RIGHT	7				//스페셜 방향
#define SPECIAL_DOWN_RIGHT	8				//	//
#define SPECIAL_UP_LEFT		9				//	//
#define SPECIAL_DOWN_LEFT	10				//	//
#define SPECIAL_RIGHT		11				//	//
#define SPECIAL_LEFT		12				//	//
#define true				1
#define false				0

struct coord {								//좌표
	int x;
	int y;
};
struct racket {
	struct coord upPos;						//라켓 멘 위의 좌표
	struct coord downPos;					//라켓 멘 밑의 좌표
	int special;                            //스페셜 무브 (1 - true, 0 - false)
};
struct ball {
	struct coord p;							//위치(position)
	int d;									//방향(direction) - 1(위+오른쪽),2(아래+오른쪽),3(위+왼쪽),4(아래+왼쪽)
};

int table[TABLE_HEIGHT][TABLE_WIDTH];		//인터페이스		(SPACE,BORDER)
int score1;									//플레이어 (1) 스코어
int score2;									//컴퓨터/플레이어2 스코어
int delay;
struct racket r1;							//플레이어			(RACKET_1)
struct racket r2;							//컴퓨터/플레이어2	(RACKET_2)
struct ball b;								//공(ball)			(BALL)

int game(int level);						//레블 0이면 2플레이어 게임이라는 뜻, 반환값은 라켓1 이기면 1이고 라켓2 이기면 0이다
void newInterface(int level);
void newBall(int direction);                //0 = 왼쪽으로 , 1 = 오른쪽으로
int r1Move(char key, int level);			//움직였는지 말지 반환값으로
int r2Move(char key);						//	//
void computerMove(int level);
void ballMove();							//공이 있는 좌표와 공의 가는 방향에 따라 공 움작임
int score(int player);                     	//스코어 추가: 1 = player (1), 2 = player 2/Computer
void moveBall(int direction);				//ballMove라는 함수에서 테스트 다 한 후 moveBall 함수에서 공 움직임
void r1MoveUp();							//라켓1 위로 움직임
void r1MoveDown();							//라켓1 아래로 움직임
void r2MoveUp();							//라켓2 위로 움직임
void r2MoveDown();							//라켓2 아래로 움직임
void pause();								//PAUSE (P)
void printInterface();						//인터페이스 출력
void menu();								//게임 모드 선택

void main() {
	menu();
}
int game(int level) {
	int i;
	char key = '\0';
	newInterface(level);
	while (key != 27 && score1 != 10 && score2 != 10) {
		system("cls");
		printInterface();
		fflush(stdin);
		key = '\0';
		if (delay == true) {
			for (i = 0; i < SCREEN_POSITION; i++)printf("\t");
			for (i = 0; i < (TABLE_WIDTH / 2) - 1; i++)printf(" ");
			printf("READY?\n");
			for (i = 0; i < STANDARD_DELAY * 10; i++)				//antes tava *20
				if (_kbhit())					//키 누르면
					key = _getch();
		}
		delay = false;
		for (i = 0; i < STANDARD_DELAY / (level == 0 ? 1 : (level*level)); i++)
			if (_kbhit())					//키 누르면
				key = _getch();
		if (level == 0) {
			if ((b.d == UP_RIGHT) || (b.d == DOWN_RIGHT) || (b.d == RIGHT) || (b.d == SPECIAL_UP_RIGHT) || (b.d == SPECIAL_DOWN_RIGHT) || (b.d == SPECIAL_RIGHT)) {
				if (!r2Move(key))
					r1Move(key, level);
			}
			else if (!r1Move(key, level))
				r2Move(key);
		}
		else {
			r1Move(key, level);
			computerMove(level);
		}
		ballMove();
	}
	if (score1 == 10) return 1;
	else return 0;
}
void newInterface(int level) {
	int i, j;
	for (i = 0; i < TABLE_HEIGHT; i++)
		for (j = 0; j < TABLE_WIDTH; j++)
			if (i == 0 || i == TABLE_HEIGHT - 1)
				table[i][j] = BORDER;			//가장자리
			else table[i][j] = SPACE;			//공간 (빈자리)
	newBall(0);					            //왼쪽+아래로 가는 공 멘가운데에서 초기화시키기
	r1.upPos.x = 0;							//라켓1 멘왼쪽가운데에서 초기화시키기
	r1.upPos.y = (TABLE_HEIGHT / 2) - LENGTH;		//	//
	r1.downPos.x = 0;							//	//
	r1.downPos.y = (TABLE_HEIGHT / 2) + LENGTH;	//	//
	for (i = r1.upPos.y; i <= r1.downPos.y; i++)	//	//
		table[i][0] = RACKET_1;				//	//
	r1.special = false;
	r2.upPos.x = TABLE_WIDTH - 1;				//라켓2 멘왼쪽가운데에서 초기화시키기
	r2.upPos.y = (TABLE_HEIGHT / 2) - LENGTH;		//	//
	r2.downPos.x = TABLE_WIDTH - 1;				//	//
	r2.downPos.y = (TABLE_HEIGHT / 2) + LENGTH;	//	//
	for (i = r2.upPos.y; i <= r2.downPos.y; i++)	//	//
		table[i][TABLE_WIDTH - 1] = RACKET_2;	//	//
	r2.special = false;
	score1 = 0;								//스코어 다시 시작
	score2 = 0;								//	//
	delay = true;								//Put delay
}
void newBall(int direction) {
	srand(time(NULL));
	b.p.x = (TABLE_WIDTH / 2) + 1;
	b.p.y = (rand() % (TABLE_HEIGHT - 2)) + 1;
	table[b.p.y][b.p.x] = BALL;
	if (direction == 0)
		if (rand() % 2)
			b.d = DOWN_LEFT;
		else
			b.d = UP_LEFT;
	else
		if (rand() % 2)
			b.d = DOWN_RIGHT;
		else
			b.d = UP_RIGHT;
}
int r1Move(char key, int level) {
	int i = 0;
	if (level == 0 && r2.special == true) {
		for (i = r2.upPos.y; i <= r2.downPos.y; i++)
			table[i][TABLE_WIDTH - 1] = RACKET_2;
		r2.upPos.x = TABLE_WIDTH - 1;
		r2.downPos.x = TABLE_WIDTH - 1;
		for (i = r2.upPos.y; i <= r2.downPos.y; i++)
			table[i][TABLE_WIDTH - 2] = SPACE;
		r2.special = false;
	}
	if (r1.special == true) {
		for (i = r1.upPos.y; i <= r1.downPos.y; i++)
			table[i][0] = RACKET_1;
		r1.upPos.x = 0;
		r1.downPos.x = 0;
		for (i = r1.upPos.y; i <= r1.downPos.y; i++)
			table[i][1] = SPACE;
		r1.special = false;
		return false;
	}
	else {
		switch (key) {
		case 'A':
		case 'a':
			r1MoveUp();
			break;
		case 'Z':
		case 'z':
			r1MoveDown();
			break;
		case 'Q':
		case 'q':
			r1MoveUp();
			r1MoveUp();
			break;
		case 'X':
		case 'x':
			r1MoveDown();
			r1MoveDown();
			break;
		case 'S':
		case 's':
			if (table[b.p.y][b.p.x - 1] == RACKET_1 && b.p.x == 1) {
				if (r1.upPos.y == b.p.y)
					b.d = SPECIAL_UP_RIGHT;
				else if (r1.downPos.y == b.p.y)
					b.d = SPECIAL_DOWN_RIGHT;
				else
					b.d = SPECIAL_RIGHT;
				for (i = r1.upPos.y; i <= r1.downPos.y; i++)
					table[i][0] = SPACE;
				r1.upPos.x = 1;
				r1.downPos.x = 1;
				for (i = r1.upPos.y; i <= r1.downPos.y; i++)
					table[i][1] = RACKET_1;
				r1.special = true;
			}
			break;
		case 'P':
		case 'p':
			pause();
			break;
		default: return false;
		}
	}
	return true;
}
int r2Move(char key) {
	int i = 0;
	if (r1.special == true) {
		for (i = r1.upPos.y; i <= r1.downPos.y; i++)
			table[i][0] = RACKET_1;
		r1.upPos.x = 0;
		r1.downPos.x = 0;
		for (i = r1.upPos.y; i <= r1.downPos.y; i++)
			table[i][1] = SPACE;
		r1.special = false;
	}
	if (r2.special == true) {
		for (i = r2.upPos.y; i <= r2.downPos.y; i++)
			table[i][TABLE_WIDTH - 1] = RACKET_2;
		r2.upPos.x = TABLE_WIDTH - 1;
		r2.downPos.x = TABLE_WIDTH - 1;
		for (i = r2.upPos.y; i <= r2.downPos.y; i++)
			table[i][TABLE_WIDTH - 2] = SPACE;
		r2.special = false;
		return false;
	}
	else {
		switch (key) {
		case '8':
			r2MoveUp();
			break;
		case '2':
			r2MoveDown();
			break;
		case '7':
			r2MoveUp();
			r2MoveUp();
			break;
		case '1':
			r2MoveDown();
			r2MoveDown();
			break;
		case '4':
			if (table[b.p.y][b.p.x + 1] == RACKET_2 && b.p.x == TABLE_WIDTH - 2) {
				if (r2.upPos.y == b.p.y)
					b.d = SPECIAL_UP_LEFT;
				else if (r2.downPos.y == b.p.y)
					b.d = SPECIAL_DOWN_LEFT;
				else
					b.d = SPECIAL_LEFT;
				for (i = r2.upPos.y; i <= r2.downPos.y; i++)
					table[i][TABLE_WIDTH - 1] = SPACE;
				r2.upPos.x = TABLE_WIDTH - 2;
				r2.downPos.x = TABLE_WIDTH - 2;
				for (i = r2.upPos.y; i <= r2.downPos.y; i++)
					table[i][TABLE_WIDTH - 2] = RACKET_2;
				r2.special = true;
			}
			break;
		case 'P':
		case 'p':
			pause();
			break;
		default:;
		}
	}
	return true;
}
void computerMove(int level) {
	int i = 0;
	srand(time(NULL));
	if (r2.special == true) {
		for (i = r2.upPos.y; i <= r2.downPos.y; i++)
			table[i][TABLE_WIDTH - 1] = RACKET_2;
		r2.upPos.x = TABLE_WIDTH - 1;
		r2.downPos.x = TABLE_WIDTH - 1;
		for (i = r2.upPos.y; i <= r2.downPos.y; i++)
			table[i][TABLE_WIDTH - 2] = SPACE;
		r2.special = false;
	}
	else if (b.p.x > (TABLE_WIDTH * 3 / 4) && (rand() % ((level - 6)*-1) == 0)) {
		if (b.p.y > r2.downPos.y)
			r2MoveDown();
		else if (b.p.y < r2.upPos.y)
			r2MoveUp();
		else if (table[b.p.y][b.p.x + 1] == RACKET_2 && b.p.x == TABLE_WIDTH - 2 && (rand() % ((level - 6)*-1) == 0)) {
			if (r2.upPos.y == b.p.y)
				b.d = SPECIAL_UP_LEFT;
			else if (r2.downPos.y == b.p.y)
				b.d = SPECIAL_DOWN_LEFT;
			else
				b.d = SPECIAL_LEFT;
			for (i = r2.upPos.y; i <= r2.downPos.y; i++)
				table[i][TABLE_WIDTH - 1] = SPACE;
			r2.upPos.x = TABLE_WIDTH - 2;
			r2.downPos.x = TABLE_WIDTH - 2;
			for (i = r2.upPos.y; i <= r2.downPos.y; i++)
				table[i][TABLE_WIDTH - 2] = RACKET_2;
			r2.special = true;
		}
	}
	else
		if (b.p.x % ((level - 6)*-1) == 0)					//soh nas coordenadas pares
			if (b.p.y > r2.downPos.y)
				r2MoveDown();
			else if (b.p.y < r2.upPos.y)
				r2MoveUp();
}
void ballMove() {
	switch (b.d) {
	case UP_RIGHT:
		if (b.p.x + 1 == TABLE_WIDTH)
			score(1);
		else if (table[b.p.y - 1][b.p.x + 1] == SPACE)
			if (table[b.p.y][b.p.x + 1] == RACKET_2)	//verifica se ao lado eh raquete, se sim, rebate normal
				moveBall(UP_LEFT);
			else
				moveBall(UP_RIGHT);		//ball moves straight
		else if ((b.p.x + 1 == r2.downPos.x&&b.p.y - 1 == r2.downPos.y&&table[b.p.y - 1][b.p.x] != BORDER) || (table[b.p.y - 1][b.p.x] == BORDER && table[b.p.y][b.p.x + 1] == RACKET_2))//se for a ponta da raquete e abaixo nao for borda, ou se for prensada de borda e raquete, rebate inverso
			moveBall(DOWN_LEFT);
		else if (table[b.p.y - 1][b.p.x] == BORDER)				//ball hits - borda
			if (table[b.p.y + 1][b.p.x + 1] == RACKET_2)
				moveBall(DOWN_LEFT);
			else moveBall(DOWN_RIGHT);
		else if (table[b.p.y][b.p.x + 1] == RACKET_2)                            //rebate pela raquete
			moveBall(UP_LEFT);
		else {
			printf("\tERROR AT CASE UP_RIGHT IN BALLMOVE - UNKNOWN POSSIBILITY\n");
			pause();
		}
		break;
	case DOWN_RIGHT:
		if (b.p.x + 1 == TABLE_WIDTH)
			score(1);
		else if (table[b.p.y + 1][b.p.x + 1] == SPACE)
			if (table[b.p.y][b.p.x + 1] == RACKET_2)	//verifica se ao lado eh raquete, se sim, rebate normal
				moveBall(DOWN_LEFT);
			else
				moveBall(DOWN_RIGHT);		//ball moves straight
		else if ((b.p.x + 1 == r2.upPos.x&&b.p.y + 1 == r2.upPos.y&&table[b.p.y + 1][b.p.x] != BORDER) || (table[b.p.y + 1][b.p.x] == BORDER && table[b.p.y][b.p.x + 1] == RACKET_2))//se for a ponta da raquete e acima nao for borda, ou se for prensada de borda e raquete, rebate inverso
			moveBall(UP_LEFT);
		else if (table[b.p.y + 1][b.p.x] == BORDER)				//ball hits - borda
			if (table[b.p.y - 1][b.p.x + 1] == RACKET_2)
				moveBall(UP_LEFT);
			else
				moveBall(UP_RIGHT);
		else if (table[b.p.y][b.p.x + 1] == RACKET_2)            //rebate pela raquete
			moveBall(DOWN_LEFT);
		else {
			printf("\tERROR AT CASE DOWN_RIGHT IN BALLMOVE - UNKNOWN POSSIBILITY\n");
			pause();
		}
		break;
	case UP_LEFT:
		if (b.p.x == 0)
			score(2);
		else if (table[b.p.y - 1][b.p.x - 1] == SPACE)
			if (table[b.p.y][b.p.x - 1] == RACKET_1)	//verifica se ao lado eh raquete, se sim, rebate normal
				moveBall(UP_RIGHT);
			else
				moveBall(UP_LEFT);		//ball moves straight
		else if ((b.p.x - 1 == r1.downPos.x&&b.p.y - 1 == r1.downPos.y&&table[b.p.y - 1][b.p.x] != BORDER) || (table[b.p.y - 1][b.p.x] == BORDER && table[b.p.y][b.p.x - 1] == RACKET_1))//se for a ponta da raquete e abaixo nao for borda, ou se for prensada de borda e raquete, rebate inverso
			moveBall(DOWN_RIGHT);
		else if (table[b.p.y - 1][b.p.x] == BORDER)				//ball hits - borda
			if (table[b.p.y + 1][b.p.x - 1] == RACKET_1)
				moveBall(DOWN_RIGHT);
			else
				moveBall(DOWN_LEFT);
		else if (table[b.p.y][b.p.x - 1] == RACKET_1)                             //rebate pela raquete
			moveBall(UP_RIGHT);
		else {
			printf("\tERROR AT CASE UP_LEFT IN BALLMOVE - UNKNOWN POSSIBILITY\n");
			pause();
		}
		break;
	case DOWN_LEFT:
		if (b.p.x == 0)
			score(2);
		else if (table[b.p.y + 1][b.p.x - 1] == SPACE)
			if (table[b.p.y][b.p.x - 1] == RACKET_1)	//verifica se ao lado eh raquete, se sim, rebate normal
				moveBall(DOWN_RIGHT);
			else
				moveBall(DOWN_LEFT);		//ball moves straight
		else if ((b.p.x - 1 == r1.upPos.x&&b.p.y + 1 == r1.upPos.y&&table[b.p.y + 1][b.p.x] != BORDER) || (table[b.p.y + 1][b.p.x] == BORDER && table[b.p.y][b.p.x - 1] == RACKET_1))//se for a ponta da raquete e acima nao for borda, ou se for prensada de borda e raquete, rebate inverso
			moveBall(UP_RIGHT);
		else if (table[b.p.y + 1][b.p.x] == BORDER)				//ball hits - borda
			if (table[b.p.y - 1][b.p.x - 1] == RACKET_1)
				moveBall(UP_RIGHT);
			else
				moveBall(UP_LEFT);
		else if (table[b.p.y][b.p.x - 1] == RACKET_1)                            //rebate pela raquete
			moveBall(DOWN_RIGHT);
		else {
			printf("\tERROR AT CASE DOWN_LEFT IN BALLMOVE - UNKNOWN POSSIBILITY\n");
			pause();
		}
		break;
	case RIGHT:
		if (b.p.x + 1 == TABLE_WIDTH)
			score(1);
		else if (table[b.p.y][b.p.x + 1] == RACKET_2)
			if (b.p.y == r2.upPos.y)
				moveBall(UP_LEFT);
			else if (b.p.y == r2.downPos.y)
				moveBall(DOWN_LEFT);
			else
				moveBall(LEFT);
		else
			moveBall(RIGHT);
		break;
	case LEFT:
		if (b.p.x - 1 == -1)
			score(2);
		else if (table[b.p.y][b.p.x - 1] == RACKET_1)
			if (b.p.y == r1.upPos.y)
				moveBall(UP_RIGHT);
			else if (b.p.y == r1.downPos.y)
				moveBall(DOWN_RIGHT);
			else
				moveBall(RIGHT);
		else
			moveBall(LEFT);
		break;
	case SPECIAL_UP_RIGHT:
		if (b.p.x + 3 >= TABLE_WIDTH)
			if (b.p.x + 1 == TABLE_WIDTH)
				score(1);
			else if (table[b.p.y - 1][b.p.x + 2] == RACKET_2)	//verifica em L eh raquete, se sim, rebate
				if (table[b.p.y - 2][b.p.x] == BORDER)       //verifica se 2 acime é borda, se for rebate, parando na mesma posição, senão rebate parando 2 acima
					b.d = DOWN_LEFT;
				else {
					moveBall(UP_RIGHT);
					moveBall(UP_LEFT);
				}
			else if (table[b.p.y - 1][b.p.x + 1] == RACKET_2 || table[b.p.y][b.p.x + 1] == RACKET_2)
				if (table[b.p.y][b.p.x + 1] == SPACE)
					moveBall(DOWN_LEFT);
				else
					moveBall(UP_LEFT);
			else if (table[b.p.y - 1][b.p.x] == BORDER)
				if (table[b.p.y + 1][b.p.x + 2] == RACKET_2) {
					moveBall(DOWN_RIGHT);
					moveBall(DOWN_LEFT);
				}
				else
					moveBall(SPECIAL_DOWN_RIGHT);
			else if (b.p.x + 2 == TABLE_WIDTH)
				score(1);
			else
				moveBall(SPECIAL_UP_RIGHT);
		else if (table[b.p.y - 1][b.p.x] == BORDER)
			moveBall(SPECIAL_DOWN_RIGHT);
		else
			moveBall(SPECIAL_UP_RIGHT);
		break;
	case SPECIAL_DOWN_RIGHT:
		if (b.p.x + 3 >= TABLE_WIDTH)
			if (b.p.x + 1 == TABLE_WIDTH)
				score(1);
			else if (table[b.p.y + 1][b.p.x + 2] == RACKET_2)	//verifica em L eh raquete, se sim, rebate
				if (table[b.p.y + 2][b.p.x] == BORDER)       //verifica se 2 abaixo é borda, se for rebate, parando na mesma posição, senão rebate parando 2 abaixo
					b.d = UP_LEFT;
				else {
					moveBall(DOWN_RIGHT);
					moveBall(DOWN_LEFT);
				}
			else if (table[b.p.y + 1][b.p.x + 1] == RACKET_2 || table[b.p.y][b.p.x + 1] == RACKET_2)
				if (table[b.p.y][b.p.x + 1] == SPACE)
					moveBall(UP_LEFT);
				else
					moveBall(DOWN_LEFT);
			else if (table[b.p.y + 1][b.p.x] == BORDER)
				if (table[b.p.y - 1][b.p.x + 2] == RACKET_2) {
					moveBall(UP_RIGHT);
					moveBall(UP_LEFT);
				}
				else
					moveBall(SPECIAL_UP_RIGHT);
			else if (b.p.x + 2 == TABLE_WIDTH)
				score(1);
			else
				moveBall(SPECIAL_DOWN_RIGHT);
		else if (table[b.p.y + 1][b.p.x] == BORDER)
			moveBall(SPECIAL_UP_RIGHT);
		else
			moveBall(SPECIAL_DOWN_RIGHT);
		break;
	case SPECIAL_UP_LEFT:
		if (b.p.x - 3 <= -1)
			if (b.p.x == 0)
				score(2);
			else if (table[b.p.y - 1][b.p.x - 2] == RACKET_1)	//verifica em L eh raquete, se sim, rebate
				if (table[b.p.y - 2][b.p.x] == BORDER)       //verifica se 2 acime é borda, se for rebate, parando na mesma posição, senão rebate parando 2 acima
					b.d = DOWN_RIGHT;
				else {
					moveBall(UP_LEFT);
					moveBall(UP_RIGHT);
				}
			else if (table[b.p.y - 1][b.p.x - 1] == RACKET_1 || table[b.p.y][b.p.x - 1] == RACKET_1)
				if (table[b.p.y][b.p.x - 1] == SPACE)
					moveBall(DOWN_RIGHT);
				else
					moveBall(UP_RIGHT);
			else if (table[b.p.y - 1][b.p.x] == BORDER)
				if (table[b.p.y + 1][b.p.x - 2] == RACKET_1) {
					moveBall(DOWN_LEFT);
					moveBall(DOWN_RIGHT);
				}
				else
					moveBall(SPECIAL_DOWN_LEFT);
			else if (b.p.x - 1 == 0)
				score(2);
			else
				moveBall(SPECIAL_UP_LEFT);
		else if (table[b.p.y - 1][b.p.x] == BORDER)
			moveBall(SPECIAL_DOWN_LEFT);
		else
			moveBall(SPECIAL_UP_LEFT);
		break;
	case SPECIAL_DOWN_LEFT:
		if (b.p.x - 3 <= -1)
			if (b.p.x == 0)
				score(2);
			else if (table[b.p.y + 1][b.p.x - 2] == RACKET_1)	//verifica em L eh raquete, se sim, rebate
				if (table[b.p.y + 2][b.p.x] == BORDER)       //verifica se 2 abaixo é borda, se for rebate, parando na mesma posição, senão rebate parando 2 abaixo
					b.d = UP_RIGHT;
				else {
					moveBall(DOWN_LEFT);
					moveBall(DOWN_RIGHT);
				}
			else if (table[b.p.y + 1][b.p.x - 1] == RACKET_1 || table[b.p.y][b.p.x - 1] == RACKET_1)
				if (table[b.p.y][b.p.x - 1] == SPACE)
					moveBall(UP_RIGHT);
				else
					moveBall(DOWN_RIGHT);
			else if (table[b.p.y + 1][b.p.x] == BORDER)
				if (table[b.p.y - 1][b.p.x - 2] == RACKET_1) {
					moveBall(UP_LEFT);
					moveBall(UP_RIGHT);
				}
				else
					moveBall(SPECIAL_UP_LEFT);
			else if (b.p.x - 1 == 0)
				score(2);
			else
				moveBall(SPECIAL_DOWN_LEFT);
		else if (table[b.p.y + 1][b.p.x] == BORDER)
			moveBall(SPECIAL_UP_LEFT);
		else
			moveBall(SPECIAL_DOWN_LEFT);
		break;
	case SPECIAL_RIGHT:
		if (b.p.x + 3 >= TABLE_WIDTH) {
			if (b.p.x + 1 == TABLE_WIDTH)
				score(1);
			else if (table[b.p.y][b.p.x + 2] == RACKET_2) {	//verifica se ao lado eh raquete, se sim, rebate de acordo com a posição
				if (b.p.y == r2.upPos.y) {
					moveBall(RIGHT);
					moveBall(UP_LEFT);
				}
				else if (b.p.y == r2.downPos.y) {
					moveBall(RIGHT);
					moveBall(DOWN_LEFT);
				}
				else {
					moveBall(RIGHT);
					b.d = LEFT;
				}
			}
			else if (table[b.p.y][b.p.x + 1] == RACKET_2) {	//	//
				if (b.p.y == r2.upPos.y) {
					moveBall(UP_LEFT);
				}
				else if (b.p.y == r2.downPos.y) {
					moveBall(DOWN_LEFT);
				}
				else {
					moveBall(LEFT);
				}
			}
			else score(1);
		}
		else {
			moveBall(SPECIAL_RIGHT);
		}
		break;
	case SPECIAL_LEFT:
		if (b.p.x - 3 <= -1) {
			if (b.p.x - 1 == -1)
				score(2);
			else if (table[b.p.y][b.p.x - 2] == RACKET_1) {	//verifica se ao lado eh raquete, se sim, rebate de acordo com a posição
				if (b.p.y == r1.upPos.y) {
					moveBall(LEFT);
					moveBall(UP_RIGHT);
				}
				else if (b.p.y == r1.downPos.y) {
					moveBall(LEFT);
					moveBall(DOWN_RIGHT);
				}
				else {
					moveBall(LEFT);
					b.d = RIGHT;
				}
			}
			else if (table[b.p.y][b.p.x - 1] == RACKET_1) {	//	//
				if (b.p.y == r1.upPos.y) {
					moveBall(UP_RIGHT);
				}
				else if (b.p.y == r1.downPos.y) {
					moveBall(DOWN_RIGHT);
				}
				else {
					moveBall(RIGHT);
				}
			}
			else score(2);
		}
		else {
			moveBall(SPECIAL_LEFT);
		}
		break;
	}
}
void moveBall(int direction) {
	if (r1.special == false && r2.special == false)	// Para não ficar um espaço bem no lugar anterior da bola, já que vai estar uma raquete
		table[b.p.y][b.p.x] = SPACE;
	b.d = direction;
	switch (direction) {
	case UP_RIGHT:
		table[--b.p.y][++b.p.x] = BALL;   // 공의 좌표가 바뀌면서 테이블에서도 위치가 바뀜
		break;
	case DOWN_RIGHT:
		table[++b.p.y][++b.p.x] = BALL;   //  //
		break;
	case UP_LEFT:
		table[--b.p.y][--b.p.x] = BALL;   //  //
		break;
	case DOWN_LEFT:
		table[++b.p.y][--b.p.x] = BALL;   //  //
		break;
	case RIGHT:
		table[b.p.y][++b.p.x] = BALL;     //  //
		break;
	case LEFT:
		table[b.p.y][--b.p.x] = BALL;     //  //
		break;
	case SPECIAL_UP_RIGHT:
		b.p.x = b.p.x + 2;
		table[--b.p.y][b.p.x] = BALL;     //  //
		break;
	case SPECIAL_DOWN_RIGHT:
		b.p.x = b.p.x + 2;
		table[++b.p.y][b.p.x] = BALL;     //  //
		break;
	case SPECIAL_UP_LEFT:
		b.p.x = b.p.x - 2;
		table[--b.p.y][b.p.x] = BALL;     //  //
		break;
	case SPECIAL_DOWN_LEFT:
		b.p.x = b.p.x - 2;
		table[++b.p.y][b.p.x] = BALL;     //  //
		break;
	case SPECIAL_RIGHT:
		b.p.x = b.p.x + 2;
		table[b.p.y][b.p.x] = BALL;
		break;
	case SPECIAL_LEFT:
		b.p.x = b.p.x - 2;
		table[b.p.y][b.p.x] = BALL;
		break;
	}
}
int score(int player) {
	table[b.p.y][b.p.x] = SPACE;
	if (player == 1) {
		score1++;
		newBall(1);
	}
	else {
		score2++;
		newBall(0);
	}
	delay = true;
	//game ends?->return
	return 0;
}
void r1MoveUp() {
	if (table[r1.upPos.y - 1][0] == SPACE) {
		table[r1.downPos.y--][0] = SPACE;
		table[--r1.upPos.y][0] = RACKET_1;
	}
}
void r1MoveDown() {
	if (table[r1.downPos.y + 1][0] == SPACE) {
		table[r1.upPos.y++][0] = SPACE;
		table[++r1.downPos.y][0] = RACKET_1;
	}
}
void r2MoveUp() {
	if (table[r2.upPos.y - 1][TABLE_WIDTH - 1] == SPACE) {
		table[r2.downPos.y--][TABLE_WIDTH - 1] = SPACE;
		table[--r2.upPos.y][TABLE_WIDTH - 1] = RACKET_2;
	}
}
void r2MoveDown() {
	if (table[r2.downPos.y + 1][TABLE_WIDTH - 1] == SPACE) {
		table[r2.upPos.y++][TABLE_WIDTH - 1] = SPACE;
		table[++r2.downPos.y][TABLE_WIDTH - 1] = RACKET_2;
	}
}
void menu() {
	int level,
		gameOn = true;
	char option='\0';
	do {
		system("cls");
		printf("\n\tEnter the mode(1 -> vs cmp , 0 -> 2 players): ");
		scanf_s("%d", &level);							//모드 선택 (1P/2P)
		if (level != 0) {
			printf("\n\tEnter the level(1 to 5): ");   //na versão final fazer um menu parecido com o do snakegame, mas também com opção de velocidade do jogo
			scanf_s("%d", &level);							//레블 선택
		}
		if (game(level))
			if (level == 0)
				printf("Player 1 wins!!\n");
			else
				printf("You win!! Congratulations!!\n");
		else
			if (level == 0)
				printf("Player 2 wins!!\n");
			else
				printf("Computer wins... Better luck next time!\n");
		printf("Do you want to play again? (y/n)");
		fflush(stdin);
		while (!_kbhit())					//키 누르는 것 기다린다
			if (_kbhit())
				if (option == 'y' || option == 'Y' || option == 'N' || option == 'n')
					option = _getch();
				else fflush(stdin);
		if (option == 'y' || option == 'Y') gameOn = false;
	} while (gameOn == true);
}
void pause() {
	int i;
	printf("\n");
	for (i = 0; i < SCREEN_POSITION; i++)printf("\t");
	printf("PAUSE\n\n");
	for (i = 0; i < SCREEN_POSITION; i++)printf("\t");
	printf("Ball: %d,%d\tdir: %d\n", b.p.x, b.p.y, b.d);
	for (i = 0; i < SCREEN_POSITION; i++)printf("\t");
	printf("table[b.p.y][b.p.x]: %d\n", table[b.p.y][b.p.x]);
	for (i = 0; i < SCREEN_POSITION; i++)printf("\t");
	printf("Racket 1: up - %d,%d\tdown - %d,%d\n", r1.upPos.x, r1.upPos.y, r1.downPos.x, r1.downPos.y);
	for (i = 0; i < SCREEN_POSITION; i++)printf("\t");
	printf("Racket 2: up - %d,%d\tdown - %d,%d\n", r2.upPos.x, r2.upPos.y, r2.downPos.x, r2.downPos.y);
	system("pause");
}
void printInterface() {
	int i, j, k;
	printf("\n");
	for (i = 0; i < SCREEN_POSITION; i++)printf("\t");
	for (i = 0; i < (TABLE_WIDTH / 2) - 1 - (score1 > 9 ? 1 : 0); i++)printf(" ");
	printf("%d : %d\n", score1, score2);
	for (i = 0; i < SCREEN_POSITION; i++)printf("\t");
	for (i = 0; i < TABLE_HEIGHT; i++) {
		for (j = 0; j < TABLE_WIDTH; j++) {
			switch (table[i][j]) {
			case SPACE:
				if (j == (TABLE_WIDTH / 2) + 1)
					printf(":");
				else printf(" ");
				break;
			case BORDER:
				printf("-");
				break;
			case RACKET_1:
			case RACKET_2:
				printf("|");
				break;
			case BALL:
				printf("o");
				break;
			}
		}
		printf("\n");
		for (k = 0; k < SCREEN_POSITION; k++)printf("\t");
	}
	printf("\n");
}