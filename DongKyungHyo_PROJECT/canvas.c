#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <string.h>
#include <time.h>
#include "jjuggumi.h"
#include "canvas.h"

#define DIALOG_DURATION_SEC		4

void draw(void);
void print_status(void);
void left_player(void);
void ascii_art_cong(void);
void ascii_art_over(void);
void ending(void);
// (zero-base) row행, col열로 커서 이동
void gotoxy(int col, int row) {
	COORD pos = { col,row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// row행, col열에 ch 출력
void printxy(char ch, int col, int row) {
	gotoxy(col, row);
	printf("%c", ch);
}

void left_player(void) {
	for (int i = 0; i < n_player; i++) {
		if (player[i] == false) {
			back_buf[py[i]][px[i]] = ' ';
		}
	}
}

void map_init(int n_row, int n_col) {
	// 두 버퍼를를 완전히 비우기
	for (int i = 0; i < ROW_MAX; i++) {
		for (int j = 0; j < COL_MAX; j++) {
			back_buf[i][j] = front_buf[i][j] = ' ';
		}
	}

	N_ROW = n_row;
	N_COL = n_col;
	for (int i = 0; i < N_ROW; i++) {
		// 대입문 이렇게 쓸 수 있는데 일부러 안 가르쳐줬음
		back_buf[i][0] = back_buf[i][N_COL - 1] = '*';

		for (int j = 1; j < N_COL - 1; j++) {
			back_buf[i][j] = (i == 0 || i == N_ROW - 1) ? '*' : ' ';
		}
	}
}

// back_buf[row][col]이 이동할 수 있는 자리인지 확인하는 함수
bool placable(int col, int row) {
	if (row < 0 || row >= N_ROW ||
		col < 0 || col >= N_COL ||
		back_buf[row][col] != ' ') {
		return false;
	}
	return true;
}

// 상단에 맵을, 하단에는 현재 상태를 출력
void display(void) {
	draw();
	gotoxy(0, N_ROW + 4);  // 추가로 표시할 정보가 있으면 맵과 상태창 사이의 빈 공간에 출력
	print_status();
}

void draw(void) {
	for (int row = 0; row < N_ROW; row++) {
		for (int col = 0; col < N_COL; col++) {
			if (front_buf[row][col] != back_buf[row][col]) {
				front_buf[row][col] = back_buf[row][col];
				printxy(front_buf[row][col], col, row);
			}
		}
	}
}

void print_status(void) {
	printf("no. of players left: %d\n", n_alive);
	for (int p = 0; p < n_player; p++) {
		printf("player %2d: %5s\n", p, player[p] ? "alive" : "DEAD");
	}
}

void ascii_art_cong() {
	printf("                                         _           _         _    _                     _  _ \n");
	printf("                                        | |         | |       | |  (_)                   | || |\n");
	printf("  ___   ___   _ __    __ _  _ __   __ _ | |_  _   _ | |  __ _ | |_  _   ___   _ __   ___ | || |\n");
	printf(" / __| / _ \\ | '_ \\  / _` || '__| / _` || __|| | | || | / _` || __|| | / _ \\ | '_ \\ / __|| || |\n");
	printf("| (__ | (_) || | | || (_| || |   | (_| || |_ | |_| || || (_| || |_ | || (_) || | | |\\__ \\|_||_|\n");
	printf(" \\___| \\___/ |_| |_| \\__, ||_|    \\__,_| \\__| \\__,_||_| \\__,_| \\__||_| \\___/ |_| |_||___/(_)(_)\n");
	printf("                      __/ |                                                                    \n");
	printf("                     |___/                                                                     \n");

}

void ascii_art_over() {
	printf(" _____                              \n");
	printf("|  __ \\                             \n");
	printf("| |  \\/  __ _  _ __ ___    ___    ___  __   __  ___  _ __  \n");
	printf("| | __  / _` || '_ ` _ \\  / _ \\  / _ \\ \\ \\ / / / _ \\| '__| \n");
	printf("| |_\\ \\| (_| || | | | | ||  __/ | (_) | \\ V / |  __/| |    _  _  _ \n");
	printf(" \\____/ \\__,_||_| |_| |_| \\___|  \\___/   \\_/   \\___||_|   (_)(_)(_)\n");

}

void ending(void) {
	int winner = 0;
	int left_player = 0;
	int winner_count = 0;
	for (int i = 0; i < n_player; i++) {
		winner_count += player[i];
		if (player[i]) {
			winner = i;
		}
	}
	system("cls");
	printf("======================================================\n\n"); Sleep(100);
	printf("             쭈꾸미 게임이 종료 되었습니다.\n\n"); Sleep(100);
	printf("======================================================\n\n"); Sleep(100);
	if (winner_count == 1) {
		printf("우승자는 플레이어 %d 입니다!\n", winner); Sleep(1000);
		ascii_art_cong();
	}
	else {
		for (int i = 0; i < n_player; i++) {
			if (player[i]) {
				printf("player%d ", i);
			}
		}
		printf("SURVIVE!\n");
		printf("우승자를 가리지 못했습니다.\n"); Sleep(1000);
		ascii_art_over();
	}
}

//dialog 구현 
char backup[ROW_MAX][COL_MAX];

void dialog_mugunghwa(char message[],char message2[], char out_player[],int size) {
	for (int i = 0; i < N_ROW; i++) {
		for (int j = 0; j < N_COL; j++) {
			backup[i][j] = back_buf[i][j];
		}
	} //전 화면 복사해놓기

	int message_row = N_ROW / 2;
	//메시지 출력할 곳

	int time = DIALOG_DURATION_SEC;

	while (time >= 0) {
		if (time > 0) {
			//메시지 칸 들어갈 곳에 있는 거 다 없애기
			for (int i = 1; i < 34; i++) {
				for (int j = -2; j <= 2; j++) {
					printxy(' ', i, message_row + j);
				}
			}

			//메시지 창 출력
			gotoxy(1, message_row - 2);
			for (int i = 0; i < 33; i++) {
				printf("*");
			} //위쪽 * 출력
			for (int j = -1; j <= 1; j++) {
				gotoxy(1, message_row+j);
				for (int i = 0; i < 1; i++) {
					printf("*");
				}
			}// 메시지 앞 * 출력
			for (int j = -1; j <= 1; j++) {
				gotoxy(33, message_row+j); //dialog 뒤 * 출력
				for (int i = 0; i < 1; i++) {
					printf("*");
				}
			}

			gotoxy(3, message_row); //남은 시간 출력

			printf("%d ", time);
			printf("%s ", message);

			for (int i = 0; i < size-1; i++) { 
				if (i%2==1) {
					printf("%c ", out_player[i]);
				}
				else {
					printf("%c", out_player[i]);
				}
			}
			printf(" %s", message2);

			gotoxy(1, message_row + 2); //아래쪽 * 출력
			for (int i = 0; i < 33; i++) {
				printf("*");
			}

			Sleep(1000); //1초 대기
		}
		else if (time == 0) { //남은 시간이 0일때 메시지창 없애기
			for (int i = 1; i < 40; i++) {
				for (int j = -2; j <= 2; j++) {
					printxy(' ', i, message_row + j);
				}
			}
			
		}
		time--;
	}
	// 이전 화면을 복구
	for (int i = 0; i < N_ROW; i++) {
		for (int j = 0; j < N_COL; j++) {
			back_buf[i][j] = backup[i][j];
			printxy(back_buf[i][j], j, i);
		}
	}
}

void dialog(char message[]) {
	for (int i = 0; i < N_ROW; i++) {
		for (int j = 0; j < N_COL; j++) {
			backup[i][j] = back_buf[i][j];
		}
	} //전 화면 복사해놓기

	int message_row = N_ROW / 2;
	//메시지 출력할 곳

	int time = DIALOG_DURATION_SEC;

	while (time >= 0) {
		if (time > 0) {
			//메시지 칸 들어갈 곳에 있는 거 다 없애기
			for (int i = N_COL / 10 - 1; i < N_COL - N_COL / 10 + 1; i++) {
				printxy(' ', i, message_row - 1);
			}
			for (int i = N_COL / 10 - 1; i < N_COL - N_COL / 10 + 1; i++) {
				printxy(' ', i, message_row);
			}
			for (int i = N_COL / 10 - 1; i < N_COL - N_COL / 10 + 1; i++) {
				printxy(' ', i, message_row + 1);
			}

			//메시지 창 출력
			gotoxy(N_COL / 10, message_row - 1);
			for (int i = 0; i <= N_COL - N_COL / 4; i++) {
				printf("*");
			} //위쪽 * 출력

			gotoxy(N_COL / 10, message_row);
			for (int i = 0; i < 1; i++) {
				printf("*");
			} // 메시지 앞 *  출력

			gotoxy(N_COL / 10 + 4, message_row); //남은 시간 출력
			printf("%d", time);
			printf("%s", message);

			gotoxy(N_COL - N_COL / 10 - 2, message_row); //dialog 뒤 * 출력
			for (int i = 0; i < 1; i++) {
				printf("*");
			}

			gotoxy(N_COL / 10, message_row + 1); //아래쪽 * 출력
			for (int i = 0; i <= N_COL - N_COL / 4; i++) {
				printf("*");
			}

			Sleep(1000); //1초 대기
		}
		else if (time == 0) { //남은 시간이 0일때 메시지창 없애기
			for (int i = N_COL / 10 - 1; i < N_COL - N_COL / 10 + 1; i++) {
				printxy(' ', i, message_row - 1);
			}
			for (int i = N_COL / 10 - 1; i < N_COL - N_COL / 10 + 1; i++) {
				printxy(' ', i, message_row);
			}
			for (int i = N_COL / 10 - 1; i < N_COL - N_COL / 10 + 1; i++) {
				printxy(' ', i, message_row + 1);
			}
			draw();
		}
		time--;
	}
	// 이전 화면을 복구
	for (int i = 0; i < N_ROW; i++) {
		for (int j = 0; j < N_COL; j++) {
			back_buf[i][j] = backup[i][j];
			printxy(back_buf[i][j], j, i);
		}
	}
}
