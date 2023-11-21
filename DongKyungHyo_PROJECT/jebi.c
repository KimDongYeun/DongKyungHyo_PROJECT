#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define DIR_LEFT	1
#define DIR_RIGHT	0
int cntt = 0;
int survive_player[PLAYER_MAX];
int ran_jebi[PLAYER_MAX];
int sum_jebi = 0;
int jx[PLAYER_MAX], jy[PLAYER_MAX];
int failer[PLAYER_MAX];
int sp=0;
int flagg = 0;
int round_j_num = 0;
int did0 = 0;
int num = 1;
int flaggg = 0;
int pick_other = 0;
int round = 1;

void one_surviv(void) // 한명 살아남은 경우
{
	int winner_num = 0;
	for (int i = 0; i < n_player; i++) {
		winner_num += player[i].is_alive;
	}
	if (winner_num == 1) {
		display();
		ending();
		exit(0);
	}
	else {
		winner_num = 0;
	}
}

void round_msg(int p) {
	gotoxy(0, N_ROW);
	printf("                                          ");
	gotoxy(0, N_ROW);
	printf("round %d, turn: player %d", round, p);
}
void jebi_init(void) {
	map_init(5, 20);
	int x, y;
	for (int i = 0; i < n_player; i++) {
		if (player[i].is_alive == true) {
			survive_player[cntt] = i;
			cntt++;
		}
	}
	round_j_num = cntt;
	for (int i = 0; i < cntt; i++) {
		y = 2;
		if (i == 0) {
			x = 2;
		}
		else {
			x = 2 + 3*i;
		}
		jx[i] = x;
		jy[i] = y;

		back_buf[jy[i]][jx[i]] = '?';  // (0 .. n_player-1)
	}
	py[survive_player[0]] = 2;
	px[survive_player[0]] = 2;
	back_buf[py[survive_player[0]]][px[survive_player[0]]] = '@';
		do {
			sum_jebi = 0;
			for (int i = 0; i < cntt; i++) {
				ran_jebi[i] = randint(0, 1);
				sum_jebi += ran_jebi[i];
			}
		} while (sum_jebi != 1);

	tick = 0;
}

// back_buf[][]에 기록
void move_tai(int player, int nx, int ny) {
	int p = player;  // 이름이 길어서...
	back_buf[ny][nx] = back_buf[py[p]][px[p]];
	back_buf[py[p]][px[p]] = '?';
	px[p] = nx;
	py[p] = ny;
}

void pick_random(int p) {
	int nx, ny;  // 움직여서 다음에 놓일 자리
	int a=0;

	do {
		a++;
		int move = randint(0, 1);
		if (move == 0) {
			nx = px[p] + 3;
		}
		else {
			nx = px[p] - 3;
		}
		ny = 2;
		if (a == 30) {
			return;
		}
	} while (!placable_j(nx, ny));

	move_tai(p, nx, ny);
}

int move_manu(key_t key) {
	// 각 방향으로 움직일 때 x, y값 delta
	static int dx[2] = { 3, -3 };
	static int dy[2] = { 0, 0 };

	int dir;  // 움직일 방향(0~3)
	sp = 0;
	switch (key) {
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;
	case K_SPACE: return 1; // sp가 1이면 스페이스바를 눌렀기에 다이얼로그 출력되고 제비 땡기기
	default: return 0;
	}

	// 움직여서 놓일 자리
	int nx, ny;
	nx = px[0] + dx[dir];
	ny = py[0] + dy[dir];
	if (!placable_j(nx, ny)) {
		return 0;
	}

	move_tai(0, nx, ny);
	return 0;
}

void mix(void) {
	do {
		sum_jebi = 0;
		for (int i = 0; i < cntt-1; i++) {
			ran_jebi[i] = randint(0, 1);
			sum_jebi += ran_jebi[i];
		}
	} while (sum_jebi != 1);
}

void next_round(void) {
	int x, y;
	cntt = 0;
	round+=1;
	for (int i = 0; i < n_player; i++) {
		if (player[i].is_alive == true) {
			survive_player[cntt] = i;
			cntt++;
		}
	}
	for (int i = 1; i < cntt+1; i++) {
		back_buf[jy[i]][jx[i]] = ' ';
	}
	for (int i = 0; i < cntt; i++) {
		y = 2;
		if (i == 0) {
			x = 2;
		}
		else {
			x = 2 + 3 * i;
		}
		jx[i] = x;
		jy[i] = y;

		back_buf[jy[i]][jx[i]] = '?';  // (0 .. n_player-1)
	}
	display();
	py[survive_player[0]] = 2;
	px[survive_player[0]] = 2;
	back_buf[py[survive_player[0]]][px[survive_player[0]]] = '@';
	round_msg(survive_player[0]);
}

void result_0()
{
	flagg = 0;
	int pick0 = 0;
		for (int i = 0; i < cntt; i++) {
			if (ran_jebi[i] == 1 && jx[i] == px[0] && jy[i] == py[0]) {
				dialog_jebi(0, "fail");
				player[0].is_alive = false;
				one_surviv();
				mix(); //실패시 제비를 다시 섞어 나머지가 플레이하게 하는 함수
				next_round();
				flagg = 1;
				num = 0;
			}
		}
		if (flagg == 0) {
			for (int i = 0; i < cntt; i++) {
				if (jx[i] == px[0] && jy[i] == py[0]) {
					pick0 = i+1;
					back_buf[jy[i]][jx[i]] = ' ';
					jx[i] = 50; //더미값
					jy[i] = 50; //더미값
				}
			}
			for (int i = pick0; i < cntt; i++) {
				back_buf[jy[i]][jx[i]] = ' ';
				jx[i] -= 3;
				back_buf[jy[i]][jx[i]] = '?';
			}
			dialog_jebi(0, "pass");
			did0 = 1;
		}
	sp = 0;
}

void result_num(void)
{
	for (int i = 0; i < cntt; i++) {
		if (ran_jebi[i] == 1 && jx[i] == px[survive_player[num]] && jy[i] == py[survive_player[num]]) {
			dialog_jebi(survive_player[num], "fail");
			player[survive_player[num]].is_alive = false;
			mix(); //실패시 제비를 다시 섞어 나머지가 플레이하게 하는 함수
			next_round();
			if (survive_player[0] != 0) {
				num = 0;
			}
			else {
				num = 1;
			}
			
			flaggg = 1;
			did0 = 0;
		}
	}
	if (flaggg == 0) {
		for (int i = 0; i < cntt; i++) {
			if (jx[i] == px[survive_player[num]] && jy[i] == py[survive_player[num]]) {
				pick_other = i+1;
				back_buf[jy[i]][jx[i]] = ' ';
				jx[i] = 50; //더미값
				jy[i] = 50; //더미값
			}
		}
		for (int i = pick_other; i < cntt; i++) {
			back_buf[jy[i]][jx[i]] = ' '; //0이 fail 시 cntt 리셋시키는 것 때문에 경고나는건데 0 pass시에만 작용하도록 조건 설정해서 오류 안남
			jx[i] -= 3;
			back_buf[jy[i]][jx[i]] = '?';
		}
		dialog_jebi(survive_player[num], "pass");
		num++;
		round_msg(survive_player[num]);
	}
	flaggg = 0;
}


void after0()
{
	tick = 0;
	py[survive_player[num]] = 2;
	px[survive_player[num]] = 2;
	back_buf[py[survive_player[num]]][px[survive_player[num]]] = '@';
	period[survive_player[num]] = 200;
	round_msg(survive_player[num]);
	display();

	while (tick<1000) {
		if (tick % period[survive_player[num]] == 0) {
			pick_random(survive_player[num]);
		}
		display();
		Sleep(10);
		tick += 10;
	}
	result_num();
}

void after0dead()
{
	tick = 0;
	py[survive_player[num]] = 2;
	px[survive_player[num]] = 2;
	back_buf[py[survive_player[num]]][px[survive_player[num]]] = '@';
	period[survive_player[num]] = 200;
	round_msg(survive_player[num]);
	display();

	while (tick < 1000) {
		if (tick % period[survive_player[num]] == 0) {
			pick_random(survive_player[num]);
		}
		display();
		Sleep(10);
		tick += 10;
	}
	result_num();
}

void jebi(void) {
		jebi_init();

		system("cls");
		round_msg(survive_player[0]);
		display();
		while (1) {
			// player 0만 손으로 움직임(4방향)
			if (survive_player[0] == 0) {
				key_t key = get_key();
				if (key == K_QUIT) {
					break;
				}
				else if (key != K_UNDEFINED) {
					sp = move_manu(key);
					if (sp == 1) {
						result_0();
					}
				}
			}
			if (did0 == 1) {
				after0();
			}
			if (survive_player[0] != 0) {
				after0dead();
			}


			one_surviv();
			display();
			Sleep(10);
			tick += 10;
		}
}