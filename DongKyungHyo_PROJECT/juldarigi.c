#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"

char out_player_jul[19] = { ' ' };
int survive_p[PLAYER_MAX];
int out_p = 0;
int cnt = 0;
int str = 0;
int key;
int hx[3], hy[3];
int p, al, di, nu = 0;
int nupgi_turns = 0;
int count = 0;
int one = 0, two = 0;
int before_str = 0;
int dx, dy, nx, ny, d;
int buhwal;

void juldarigi_init();
void p_str();
void move_player();
void move_dash();
void player_print();
void dash_print();
void after_nupgi();
void play_juldarigi();

void juldarigi_init(void) {
	map_init(3, 32);
	for (int i = 0; i < n_player; i++) {
		if (player[i].is_alive == true) {
			survive_p[cnt] = i;
			cnt++;
			n_alive++;
		}
		else if (player[i].is_alive == false) {
			player[i].is_alive = true;
			player[i].hasitem = false;
			survive_p[cnt] = i;
			cnt++;
			n_alive++;
		}
	}
	int ma = 2, pl = 2;
	for (int i = 0; i < n_player; i++) {
		py[i] = N_ROW / 2;
		if (i % 2 == 0) {
			px[i] = N_COL / 2 - 1 - ma;
			ma += 1;
		}
		else if (i % 2 != 0) {
			px[i] = N_COL / 2 - 1 + pl;
			pl += 1;
		}

		back_buf[py[i]][px[i]] = '0' + i;
	}
	for (int i = 0; i < 3; i++) {
		hy[i] = 1;
		hx[i] = N_COL / 2 - i;
		back_buf[hy[i]][hx[i]] = '-';
	}
	ma = 0, pl = 0;
	nupgi_turns = 0;
}

void p_str() { //str 계산
	int lt_str = 0, rt_str = 0;
	for (int i = 0; i < n_player; i++) {
		if (i % 2 == 0 && player[i].is_alive) {
			lt_str += player[i].str;
		}
		else if (i % 2 != 0 && player[i].is_alive) {
			rt_str += player[i].str;
		}
	}
	str = -lt_str + rt_str;
}

void move_player(int player, int nx, int ny) { //플레이어
	int p = player;  // 이름이 길어서...
	back_buf[ny][nx] = back_buf[py[p]][px[p]];
	back_buf[py[p]][px[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}

void move_dash(int ddx, int ddy, int i) { //줄
	back_buf[ddx][ddy] = back_buf[hy[i]][hx[i]];
	back_buf[hy[i]][hx[i]] = ' ';
	hx[i] = ddx;
	hy[i] = ddy;
}

void player_print() { //플레이어 출력
	for (int i = 0; i < n_player; i++) {
		if (player[i].is_alive == true) {
			back_buf[py[i]][px[i]] = '0' + i;
		}
	}
}

void dash_print() { //줄 출력
	for (int i = 0; i < 3; i++) {
		back_buf[hy[i]][hx[i]] = '-';
	}
}

void after_nupgi() {
	for (int i = 0; i < N_COL; i++) {
		printxy(' ', i, 4);
	}
	for (int i = 0; i < n_player; i++) {
		if (nu == 1 && i % 2 == 0) {
			player[i].str = player[i].str / 2;
		}
		else if (nu == 2 && i % 2 != 0) {
			player[i].str = player[i].str / 2;
		}
	}
	nu = 0;
	nupgi_turns = 0;
}

void countkey() {
	while (1) {
		if (_kbhit()) {
			char key = _getch();

			if (key == 'z') {
				count++;
				one = 1;
			}
			else if (key == '/') {
				count++;
				two = 1;
			}
		}
		Sleep(500);
		if (!_kbhit()) {
			break;
		}
	}
	return 0;
}

void play_juldarigi() {
	before_str = str;
	int nx, ny;
	countkey();
	p_str();
	if (one == 1 || two == 1) {
		if (one == 1) {
			str -= count;
		}
		else if (two == 1) {
			str += count;
		}
	}
	Sleep(1000);
	for (int i = 0; i < n_player; i++) {
		p = i;
		if (str > 0) {
			nx = px[p] + 1;
			ny = py[p];
			if (nu == 2 && nupgi_turns == 1) {
				nx = px[p] + 2;
			}
			move_player(p, nx, ny);
			player_print();
		}
		else if (str < 0) {
			nx = px[p] - 1;
			ny = py[p];
			if (nu == 1 && nupgi_turns == 1) {
				nx = px[p] - 2;
			}
			move_player(p, nx, ny);
			player_print();
		}
		if ((p % 2 == 0 && px[p] >= 15) || (p % 2 != 0 && px[p] <= 15)) {
			if (p % 2 == 0) {
				al = 1;
			}
			else if (p % 2 != 0) {
				al = 2;
			}
			di = 1;
			back_buf[1][15] = ' ';
			out_player_jul[out_p] = '0' + i;
			player[i].is_alive = false;
			n_alive -= 1;
		}
	}
	if (al == 1 || al == 2) { //죽었을때 상대편 안움직이게 할 코드
		for (int i = 0; i < n_player; i++) {
			if (i % 2 == 0 && al == 2) {
				nx = px[i] + 1;
				ny = 1;
				move_player(i, nx, ny);
				player_print();
			}
			else if (i % 2 != 0 && al == 1) {
				nx = px[i] - 1;
				ny = 1;
				move_player(i, nx, ny);
				player_print();
			}
		}
		for (int j = 0; j < 3; j++) { //죽었을때 줄 안움직이게 하는 코드
			d = j;
			dx = hx[j];
			dy = hy[j];
			if (nu != 0 && nupgi_turns == 1) {
				if (str > 0) {
					dx += 1;
				}
				else if (str < 0) {
					dx -= 1;
				}
			}
			move_dash(dx, dy, j);
			dash_print();
		}
		al = 0;
	}
	else { //죽은애 없을때 줄 움직이게 하는 코드
		int moveAmount = (str > 0) ? 1 : -1;
		for (int j = 0; j < 3; j++) {
			d = j;
			dx = hx[j] + moveAmount;
			dy = hy[j];
			if (nu != 0 && nupgi_turns == 1) {
				if (str > 0) {
					dx += 1;
				}
				else if (str < 0) {
					dx -= 1;
				}
			}
			move_dash(dx, dy, j);
			dash_print();
		}
	}
	if (nu != 0 && nupgi_turns == 1) {
		after_nupgi();
	}
	count = 0;
}

int juldarigi(void) {
	juldarigi_init();
	system("cls");
	display();
	gotoxy(N_COL / 2 - 1, 0);
	printf(" ");
	gotoxy(N_COL / 2 - 1, 2);
	printf(" ");
	gotoxy(0, 5);
	printf("str :	%d", str);
	printf("\n");
	while (1) {
		if (_kbhit()) {
			int key = _getch();
			if (key == 'q') { break; }
			else if (key == 'x') {
				int ifff = 0;
				nu = 1;
				gotoxy(0, 4);
				printf("왼쪽 팀이 눕기를 사용했습니다.\n");
				for (int i = 0; i < n_player; i++) {
					if (i % 2 == 0 && player[i].stamina > 0) {
						player[i].str *= 2;
						player[i].stamina *= 0.7;
					}
					else if (i % 2 == 0 && player[i].stamina < 0) {
						ifff = 1;
					}
				}
				if (ifff == 1) {
					gotoxy(0, 4);
					printf("왼쪽 팀이 눕기를 사용했지만, 스테미너가 부족하여 실패하였습니다.\n");
					ifff = 0;
				}
				nupgi_turns = 1;
			}
			else if (key == '.') {
				int ifff = 0;
				gotoxy(0, 4);
				printf("오른쪽 팀이 눕기를 사용했습니다.\n");
				for (int i = 1; i < n_player; i++) {
					if (i % 2 != 0 && player[i].stamina > 0) {
						player[i].str *= 2;
						player[i].stamina *= 0.7;
					}
					else if (i % 2 != 0 && player[i].stamina < 0) {
						ifff = 1;
					}
				}
				if (ifff == 1) {
					gotoxy(0, 4);
					printf("왼쪽 팀이 눕기를 사용했지만, 스테미너가 부족하여 실패하였습니다.\n");
					ifff = 0;
				}
				nupgi_turns = 1;
				nu = 2;
			}
			else if (key == 'x' && '.') {
				gotoxy(0, 4);
				printf("왼쪽과 오른쪽 팀 둘다 눕기를 사용했습니다.\n");
				for (int i = 0; i < n_player; i++) {
					player[i].str *= 2;
					player[i].stamina *= 0.7;
					nupgi_turns = 1;
				}
				nu = 3;
			}
		}
		display();
		for (int i = 0; i < N_COL; i++) {
			printxy(' ', i, 5);
		}
		gotoxy(0, 5);
		printf("str :	%d\n", str);
		play_juldarigi();
		display();
		for (int i = 0; i < N_COL; i++) {
			printxy(' ', i, 5);
		}
		gotoxy(0, 5);
		printf("str :	%d\n", str);
		Sleep(1000);
		if (di == 1) {
			dialog_juldarigi("player", "dead!", out_player_jul, out_p);
			di = 0;
		}
		if (one == 1 || two == 1) {
			str = before_str;
			one = 0;
			two = 0;
		}
		int allEvenDead = 1; // 짝수 애들 죽었는지 확인할 코드
		int allOddDead = 1;  //홀수 애들 

		for (int i = 0; i < n_player; i++) {
			if (i % 2 == 0 && player[i].is_alive) {
				allEvenDead = 0;
			}
			else if (i % 2 != 0 && player[i].is_alive) {
				allOddDead = 0;
			}
		}

		if (allEvenDead || allOddDead) {
			break;
		}
	}
	return 0;
}