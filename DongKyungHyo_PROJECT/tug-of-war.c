#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"

int survive_p[PLAYER_MAX];
int cnt = 0;
int str = 0;
int step = 0;

void jul_init(void) {
	map_init(3, 32);
	int x, y;
	for (int i = 0; i < n_player; i++) {
		if (player[i].is_alive == true) {
			survive_p[cnt] = i;
			cnt++;
			n_alive++;
		}
		else if (player[i].is_alive == false) {
			player[i].is_alive = true;
			player[i].hasitem = false;
			n_alive++;
		}
	}
}

void print_jul() {
	gotoxy(N_COL / 2 - 5, N_ROW / 2);
	int pp = 2;
	for (int i = n_player - 1; i >= 0; i--) {
		if (i % 2 == 0) {
			printf("%d", i);
		}
	}
	for (int j = 0; j < n_player; j++) {
		if (j % 2 == 0) {
			px[j] = N_COL / 2 - pp;
			py[j] = N_ROW / 2;
			pp++;
		}
	}
	gotoxy(N_COL / 2 - 2, N_ROW / 2);
	printf("---");
	pp = 2;
	gotoxy(N_COL / 2 + 1, N_ROW / 2);
	for (int i = 0; i < n_player; i++) {
		if (i % 2 != 0) {
			px[i] = N_COL / 2 + pp;
			py[i] = N_ROW / 2;
			pp++;
			printf("%d", i);
		}
	}
}

void move_str() {
	for (int i = 2; i < N_COL - 1; i++) {
		printxy(' ', i, N_ROW / 2);
	}
	for (int i = n_player - 1; i >= 0; i--) {
		if (i % 2 == 0 && player[i].is_alive == true) {
			gotoxy(px[i], N_ROW / 2);
			printf("%d", i);
		}
	}
	printf("---");
	for (int i = 0; i < n_player; i++) {
		if (i % 2 != 0 && player[i].is_alive == true) {
			gotoxy(px[i], N_ROW / 2);
			printf("%d", i);
		}
	}
}

void printgame() {
	system("cls");
	printf("############### ################\n");
	printf("#");
	print_jul();
	printf("\n");
	printf("############### ################\n");
	printf("\n\n\n\n");
	printf("str :	%d\n", str);
	print_status();
}

void p_str() {
	while (1) {
		if (step == 0) {
			double lt_str = 0;
			double rt_str = 0;
			for (int i = 0; i < n_player; i++) {
				if (i % 2 == 0) {
					lt_str += player[i].str;
				}
				else {
					rt_str += player[i].str;
				}
			}
			str = -lt_str + rt_str;
		}
		//str = 20; //테스트 코드 (냐중에 지울예정)

		int mid_col = N_COL / 2, str_col = mid_col - 5;
		int die_ps = 0;
		if (str > 0) {
			for (int i = 0; i < n_player; i++) {
				if (player[i].is_alive) {
					int temp_px = px[i] + 1;
					if (temp_px == N_COL / 2 - 1) {
						player[i].is_alive = false;
						n_alive--;
						die_ps = player[i].str;
					}
					else {
						px[i] = temp_px;
						move_str();
					}
				}
			}
		}
		else if (str < 0) { //오른쪽 수정해야함
			for (int i = 0; i < n_player; i++) {
				if (player[i].is_alive) {
					int temp_px = px[i] - 1;
					if (temp_px == N_COL / 2 - 1) {
						player[i].is_alive = false;
						//px[i + 2] = px[i + 2] - 1;
						n_alive--;
						die_ps = player[i].str;
						printxy(' ', px[i], N_ROW / 2);
					}
					else {
						px[i] = temp_px;
						move_str();
					}
				}
			}

		}
		step += 1;
		/*for (int i = 0; i < n_player; i++) {
			printf("%d 좌표 : %d\n", i, px[i]);
		}*/ //테스트코드
		str = str + die_ps;
		for (int i = 0; i < N_COL; i++) {
			printxy(' ', i, N_ROW + 4);
		}
		gotoxy(0, N_ROW + 4);
		printf("str :	%d\n", str);
		print_status();
		Sleep(2000);
	}
}

void juldarigi(void) {
	jul_init();
	printgame();
	Sleep(1000);
	p_str();
}