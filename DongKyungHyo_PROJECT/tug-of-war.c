#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"

char out_player[19] = { ' ' };
int survive_p[PLAYER_MAX];
int out_p = 0;
int cnt = 0;
int str = 0;
int key;
int hx[3], hy[3];
int p, al, di, nu = 0;
int nupgi_turns = 0;
int bh;

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

void p_str() { //str ���
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

void move_tail(int player, int nx, int ny) { //�÷��̾�
	int p = player;  // �̸��� ��...
	back_buf[ny][nx] = back_buf[py[p]][px[p]];
	back_buf[py[p]][px[p]] = '  ';
	px[p] = nx;
	py[p] = ny;
}

void move_dash(int ddx, int ddy, int i) { //�÷��̾� ���
	back_buf[ddx][ddy] = back_buf[hy[i]][hx[i]];
	back_buf[hy[i]][hx[i]] = ' ';
	hx[i] = ddx;
	hy[i] = ddy;
}

void player_print() { //��
	for (int i = 0; i < n_player; i++) {
		if (player[i].is_alive == true) {
			back_buf[py[i]][px[i]] = '0' + i;
		}
	}
}

void dash_print() { //�� ���
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

void play_juldarigi() {
	int nx, ny;
	p_str();
	Sleep(1000);
	for (int i = 0; i < n_player; i++) {
		p = i;
		if (str > 0) {
			nx = px[p] + 1;
			ny = py[p];
			if (nu == 2 && nupgi_turns == 1) {
				nx = px[p] + 2;
			}
			move_tail(p, nx, ny);
			player_print();
			if (nu != 0 && nupgi_turns == 1) {
				after_nupgi();
			}
		}
		else if (str < 0) {
			nx = px[p] - 1;
			ny = py[p];
			if (nu == 1 && nupgi_turns == 1) {
				nx = px[p] - 2;
			}
			move_tail(p, nx, ny);
			player_print();
			if (nu != 0 && nupgi_turns == 1) {
				after_nupgi();
			}
		}
		if (px[p] == N_COL / 2 - 1 && player[p].is_alive) {
			if (p % 2 == 0) {
				al = 1;
			}
			else if (p % 2 != 0) {
				al = 2;
			}
			di = 1, out_p = 1;
			back_buf[py[p]][px[p]] = ' ';
			out_player[out_p] = '0' + p;
			player[i].is_alive = false;
			n_alive -= 1;
			out_p = i;
		}
	}
	if (al == 1 || al == 2) {
		for (int i = 0; i < n_player; i++) {
			if (i % 2 == 0 && al == 2) {
				int nx, ny;
				nx = px[i] + 1;
				ny = 1;
				move_tail(i, nx, ny);
				player_print();
			}
			else if (i % 2 != 0 && al == 1) {
				int nx, ny;
				nx = px[i] - 1;
				ny = 1;
				move_tail(i, nx, ny);
				player_print();
			}
		}
		for (int j = 0; j < 3; j++) {
			int dx, dy;
			dx = hx[j];
			dy = hy[j];
			move_dash(dx, dy, j);
			dash_print();
		}
		al = 0;
	}
	else {
		int moveAmount = (str > 0) ? 1 : -1;
		for (int j = 0; j < 3; j++) {
			int dx, dy;
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
				nu = 1;
				gotoxy(0, 4);
				printf("���� ���� ���⸦ ����߽��ϴ�.\n");
				for (int i = 0; i < n_player; i++) {
					if (i % 2 == 0) {
						player[i].str *= 2;
						player[i].stamina *= 0.7;
					}
				}
				nupgi_turns = 1;
			}
			else if (key == '.') {
				gotoxy(0, 4);
				printf("������ ���� ���⸦ ����߽��ϴ�.\n");
				for (int i = 1; i < n_player; i++) {
					if (i % 2 != 0) {
						player[i].str *= 2;
						player[i].stamina *= 0.7;
					}
				}
				nupgi_turns = 1;
				nu = 2;
			}
			else if (key == 'x' && '.') {
				gotoxy(0, 4);
				printf("���ʰ� ������ �� �Ѵ� ���⸦ ����߽��ϴ�.\n");
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
			dialog_juldarigi("player", "dead!", out_p, 2);
			di = 0;
		}
		if (n_alive == 1) {
			break;
		}
	}
}