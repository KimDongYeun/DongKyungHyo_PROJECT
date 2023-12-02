#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"

char out_player_jul[19] = { ' ' }; //������ �����Ұ�
int survive_p[PLAYER_MAX];
int out_p = 0; //������
int cnt_t = 0; //����
double str = 0.0; //��
int key; //Ű �Է�
int hx[3], hy[3]; //��
int p, diee, di, nu = 0;
int nupgi_turns = 0; //���� ��
double before_str = 0; //���� �ϱ� ���� ������ ��
int dx, dy, nx, ny, d; //��, �÷��̾� ��� ����
int buhwal[10] = { ' ' }; //������ ��� ������ �ִ� �÷��̾� �����Ұ�
int notbu; //�굵
int num_dead_player = 0;

void juldarigi_init();
void p_str();
void move_player();
void move_dash();
void player_print();
void dash_print();
void after_nupgi();
void play_juldarigi();

void juldarigi_init(void) { //��ũ�� ����, ������ ��Ȱ, �÷��̾� ��ġ ����, �� ��ġ ����
	map_init(3, 32);
	for (int i = 0; i < n_player; i++) {
		if (player[i].is_alive == true) {
			survive_p[cnt_t] = i;
			cnt_t++;
			buhwal[notbu] = i;
		}
		else if (player[i].is_alive == false) {
			player[i].is_alive = true;
			player[i].hasitem = false;
			survive_p[cnt_t] = i;
			cnt_t++;
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
	double lt_str = 0, rt_str = 0;
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

void move_player(int player, int nx, int ny) { //�÷��̾�
	int p = player;  // �̸��� ��...
	back_buf[ny][nx] = back_buf[py[p]][px[p]];
	back_buf[py[p]][px[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}

void move_dash(int ddx, int ddy, int i) { //��
	back_buf[ddx][ddy] = back_buf[hy[i]][hx[i]];
	back_buf[hy[i]][hx[i]] = ' ';
	hx[i] = ddx;
	hy[i] = ddy;
}

void player_print() { //�÷��̾� ���
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

void after_nupgi() { //���� �� �� �ʱ�ȭ��Ű��
	for (int i = 0; i < COL_MAX; i++) {
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

int keytrue = 0;
void countkey() { // ��Ÿ �ڵ�
	while (1) {
		if (_kbhit()) {
			char key = _getch();

			if (key == 'z') {
				str -= 1;
				for (int i = 0; i < N_COL; i++) {
					printxy(' ', i, 5);
				}
				gotoxy(0, 5);
				printf("str :	%.lf\n", str);
			}
			else if (key == '/') {
				str += 1;
				for (int i = 0; i < N_COL; i++) {
					printxy(' ', i, 5);
				}
				gotoxy(0, 5);
				printf("str :	%.lf\n", str);
			}
			keytrue = 1;
		}
		Sleep(400);
		if (!_kbhit()) {
			break;
		}
	}
	return 0;
}

int turn = 1;
int nup_die = 0;
int nup2_die = 0;
void play_juldarigi() { //���ӵ��ư��� ��¥ �ڵ�
	before_str = str;
	int nx, ny;
	p_str(); //str����ϱ�
	for (int i = 0; i < N_COL; i++) {
		printxy(' ', i, 5);
	}
	gotoxy(0, 5);
	printf("str :	%.lf\n", str);
	countkey(); //���� ��Ÿ�ҰŸ�
	Sleep(1000); //1�� ���
	for (int i = 0; i < n_player; i++) { //�÷��̾� �����̰��ϱ�, nu, nupgi_turns�� ���� ������ 2ĭ �����̱�
		p = i;
		if (str > 0) {
			nx = px[p] + 1;
			ny = py[p];
			if (nu == 2 && nupgi_turns == 1) {
				nx = px[p] + 2;
			}
			move_player(p, nx, ny);
		}
		else if (str < 0) {
			nx = px[p] - 1;
			ny = py[p];
			if (nu == 1 && nupgi_turns == 1) {
				nx = px[p] - 2;
			}
			move_player(p, nx, ny);
		}
		if ((p % 2 == 0 && px[p] >= 15) || (p % 2 != 0 && px[p] <= 15)) { //�׾�� �ϴ� ���
			if (player[p].is_alive) {
				if (p % 2 == 0) { //������ �� �׾�����
					diee = 1;
				}
				else if (p % 2 != 0) { //���ʾ� �׾�����
					diee = 2;
				}
				di = 1;
				back_buf[1][15] = ' ';
				out_player_jul[out_p + num_dead_player] = '0' + i;
				player[p].is_alive = false;
				num_dead_player++;
				n_alive -= 1;
				if (p % 2 == 0 && nu != 0) {
					nup_die += 1;
				}
				else if (p % 2 != 0 && nu != 0) {
					nup2_die += 1;
				}
			}
		}
	}
	if (diee == 1 || diee == 2) { //�׾����� ����� �ȿ����̰� �� �ڵ�
		for (int i = 0; i < n_player; i++) {
			if (i % 2 == 0 && diee == 2) {
				nx = px[i] + 1;
				ny = 1;
				move_player(i, nx, ny);
			}
			else if (i % 2 != 0 && diee == 1) {
				nx = px[i] - 1;
				ny = 1;
				move_player(i, nx, ny);
			}
		}
		if (nup_die != 0 && nu != 0) { //���� ������ ���� �׾����� ���� �ȿ����̰� �ϱ� .��������
			for (int i = 0; i < n_player; i++) {
				if (nup_die == 1) {
					if (i % 2 != 0) {
						nx = px[i];
						ny = 1;
						move_player(i, nx, ny);
					}
				}
				else if (nup_die == 2) {
					if (i % 2 != 0) {
						nx = px[i] - 1;
						ny = 1;
						move_player(i, nx, ny);
					}
				}
			}
		}
		else if (nup2_die != 0 && nu != 0) {//���� ������ ���� �׾����� ���� �ȿ����̰� �ϱ� x��������
			for (int i = 0; i < n_player; i++) {
				if (nup2_die == 1) {
					if (i % 2 == 0) {
						nx = px[i];
						ny = 1;
						move_player(i, nx, ny);
					}
				}
				else if (nup2_die == 2 || nup2_die == 3) {
					if (i % 2 == 0) {
						nx = px[i] + 1;
						ny = 1;
						move_player(i, nx, ny);
					}
				}
			}
		}
		if (nup_die != 0 && nu != 0) {
			for (int i = 0; i < 3; i++) {
				if (nup_die == 1) {
					dx = 15 + i;
					dy = 1;
					move_dash(dx, dy, i);
					dash_print();
				}
				else if (nup_die == 2) {
					dx = 15 + i;
					dy = 1;
					move_dash(dx, dy, i);
					dash_print();
				}
			}
			//nup_die = 0;
		}
		else if (nup2_die != 0 && nu != 0) {
			for (int i = 0; i < 3; i++) {
				if (nup2_die == 1) {
					dx = 13 + i;
					dy = 1;
					move_dash(dx, dy, i);
					dash_print();
				}
				else if (nup2_die == 2) {
					dx = 13 + i;
					dy = 1;
					move_dash(dx, dy, i);
					dash_print();
				}
			}
		}
		else {
			for (int j = 0; j < 3; j++) { //�׾����� �� �ȿ����̰� �ϴ� �ڵ�
				d = j;
				if (nu != 0 && nupgi_turns == 1) {
					if (nu == 1) {
						dx = hx[j] - 2;
						dy = 1;
					}
					else if (nu == 2) {
						dx = hx[j] + 2;
						dy = 1;
					}
				}
				else {
					dx = hx[j];
					dy = 1;
				}
				move_dash(dx, dy, j);
				dash_print();
			}
			diee = 0;
		}
	}
	else { //������ ������ �� �����̰� �ϴ� �ڵ�
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
		}
	}
	dash_print();
	player_print();
	nup_die = 0;
	nup2_die = 0;
	if (nu != 0 && nupgi_turns == 1) { //���� �� �ʱ�ȭ
		after_nupgi();
	}
}

int x_pressed = 0;
int dot_pressed = 0;
int juldarigi(void) {
	juldarigi_init();
	system("cls");
	display();
	gotoxy(N_COL / 2 - 1, 0);
	printf(" ");
	gotoxy(N_COL / 2 - 1, 2);
	printf(" ");
	gotoxy(0, 5);
	printf("str :	%.lf", str);
	printf("\n");
	while (1) {
		if (_kbhit()) {
			int key = _getch();
			if (key == 'q') { break; }
			else if (key == 'x') {
				int ifff = 0;
				nu = 1;
				if (_kbhit()) {
					key = _getch();
					if (key == '.') {
						for (int i = 0; i < n_player; i++) {
							player[i].str *= 2;
							player[i].stamina *= 0.7;
						}
						gotoxy(0, 4);
						printf("�������� �Բ� ���⸦ ����߽��ϴ�.\n");
					}
				}
				else {
					gotoxy(0, 4);
					printf("���� ���� ���⸦ ����߽��ϴ�.\n");
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
						printf("������ ü�� �������� ���� ����.\n");
						ifff = 0;
						nu = 0;
					}
				}
				nupgi_turns = 1;
				x_pressed = 1;
			}
			else if (key == '.') {
				int ifff = 0;
				nu = 2;
				if (_kbhit()) {
					key = _getch();
					if (key == 'x') {
						for (int i = 0; i < n_player; i++) {
							player[i].str *= 2;
							player[i].stamina *= 0.7;
						}
						gotoxy(0, 4);
						printf("�������� �Բ� ���⸦ ����߽��ϴ�.\n");
					}
				}
				else {
					gotoxy(0, 4);
					printf("������ ���� ���⸦ ����߽��ϴ�.\n");
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
						printf("�������� ü�� �������� ���� ����.\n");
						ifff = 0;
						nu = 0;
					}
				}
				nupgi_turns = 1;
			}
		}
		display();
		for (int i = 0; i < N_COL; i++) {
			printxy(' ', i, 5);
		}
		gotoxy(0, 5);
		printf("str :	%.lf\n", str);
		play_juldarigi();
		display();
		for (int i = 0; i < N_COL; i++) {
			printxy(' ', i, 5);
		}
		gotoxy(0, 5);
		printf("str :	%.lf\n", str);
		Sleep(1000);
		if (di == 1) {
			if (num_dead_player == 1) {
				dialog_juldarigi("player", "dead!", out_player_jul, 1);
			}
			else if (num_dead_player >= 2) {
				dialog_juldarigi("player", "dead!", out_player_jul, num_dead_player);
			}
			di = 0;
			num_dead_player = 0;
			Sleep(200);
		}
		if (keytrue == 1) {
			str = before_str;
			for (int i = 0; i < N_COL; i++) {
				printxy(' ', i, 5);
			}
			gotoxy(0, 5);
			printf("str :	%.lf\n", str);
			keytrue = 0;
		}
		int allEvenDead = 1; // ¦�� �ֵ� �׾����� Ȯ���� �ڵ�
		int allOddDead = 1;  //Ȧ�� �ֵ� 

		for (int i = 0; i < n_player; i++) {
			if (i % 2 == 0 && player[i].is_alive) {
				allEvenDead = 0;
			}
			else if (i % 2 != 0 && player[i].is_alive) {
				allOddDead = 0;
			}
		}

		if (allEvenDead || allOddDead) { //ó���� ��Ҿ��� �� ������ �ٽ� �츮��, ������ ���ְ� ��,���� ���� �϶� �� �ٴٸ��� ����
			for (int i = 0; i < n_player; i++) {
				if (i == buhwal[notbu]) {
					player[i].is_alive = true;
					player[i].str = player[i].str / 2;
					player[i].intel = player[i].intel / 2;
					player[i].hasitem = false;
				}
			}
			break;
		}
	}
	return 0;
}