#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>


#define DIR_UP		2
#define DIR_DOWN	3
#define DIR_LEFT	1
#define DIR_RIGHT	0
#define CHA		1
#define IGN		2
#define STR		1
#define INT		2
#define BYE		3
int cnt = 0;
int survive_p[PLAYER_MAX];
int item_cnt;
ITEM night_item[ITEM_MAX];
int item_num[4];


void nightgame_init(void) {
	map_init(8, 23);
	int x, y;
	for (int i = 0; i < n_player; i++) {
		if (player[i].is_alive == true) {
			survive_p[cnt] = i;
			cnt++;
		}
	}
	for (int i = 0; i < cnt; i++) {
		// 같은 자리가 나오면 다시 생성
		do {
			y = randint(1, N_ROW - 2);
			x = randint(1, N_COL - 2);
		} while (!placable(x, y));
		px[survive_p[i]] = x;
		py[survive_p[i]] = y;
		period[survive_p[i]] = randint(300, 500);

		back_buf[py[survive_p[i]]][px[survive_p[i]]] = '0' + survive_p[i];  // (0 .. n_player-1)
	}
	item_cnt = randint(1, 4);
	for (int i = 0; i < item_cnt; i++) {
		item_num[i] = randint(0, ITEM_MAX-1);
		for (int j = 0; j < i; j++) {
			if (item_num[i] == item_num[j]) { i--; break; }
		}
	}
	
	for (int i = 0; i < item_cnt; i++) {
		do {
			y = randint(1, N_ROW - 2);
			x = randint(1, N_COL - 2);
		} while (!placable(x, y));
		night_item[i] = item[item_num[i]]; //
		ix[i] = x;
		iy[i] = y;

		back_buf[iy[i]][ix[i]] = 'I';
	}
	

	tick = 0;
}

// back_buf[][]에 기록
void move_record(int player, int nx, int ny) {
	int p = player;  // 이름이 길어서...
	back_buf[ny][nx] = back_buf[py[p]][px[p]];
	back_buf[py[p]][px[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}

void move_0_manual(key_t key) {
	// 각 방향으로 움직일 때 x, y값 delta
	static int dx[4] = { 1, -1, 0, 0 };
	static int dy[4] = { 0, 0, -1, 1 };

	int dir;  // 움직일 방향(0~3)
	switch (key) {
	case K_UP: dir = DIR_UP; break;
	case K_DOWN: dir = DIR_DOWN; break;
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;
	default: return;
	}

	// 움직여서 놓일 자리
	int nx, ny;
	nx = px[0] + dx[dir];
	ny = py[0] + dy[dir];
	if (!placable(nx, ny)) {
		return;
	}

	move_record(0, nx, ny);
}

void move_rand(int playerr, int dir) {
	int p = playerr;  // 이름이 길어서...
	int nx, ny;  // 움직여서 다음에 놓일 자리
	int flag = 0;
	int someone = 0;
	// 움직일 공간이 없는 경우는 없다고 가정(무한 루프에 빠짐)	

	do { 
		for (int j = -1; j <= 1; j += 2) { // 3X3에서 대각선에 I 있을 시 근처로 이동
			for (int i = -1; i <= 1; i += 2) {
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + j][px[p] + i] == 'I') {
						nx = px[p] + i;
						ny = py[p];
						flag = 1;
						if (placable(nx, ny) == false) {
							nx = px[p];
							ny = py[p] + j;
							flag = 1;
						}
					}
					else if (back_buf[py[p] + j][px[p] + i] == '0' + survive_p[k]&& player[survive_p[k]].hasitem == true&&flag==0) {
							nx = px[p] + i;
							ny = py[p];
							flag = 1;
							if (placable(nx, ny) == false) {
								nx = px[p];
								ny = py[p] + j;
								flag = 1;
							}
					}
				}
			}
		}
		if (flag == 0) {
			for (int i = -2; i <0; i++) { //5X5에서 1열에 1행2행에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] - 2] == 'I') {
						nx = px[p] - 1;
						ny = py[p] - 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] - 2] == '0' + survive_p[k]&& player[survive_p[k]].hasitem == true&&flag==0) {
							nx = px[p] - 1;
							ny = py[p] - 1;
							flag = 1;
					}
				}
			}
			for (int k = 0; k < cnt; k++) {
				if (back_buf[py[p]][px[p] - 2] == 'I') { //5X5에서 1열에 3행에 I 존재시 이동
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
				else if (back_buf[py[p]][px[p] - 2] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true&& flag == 0) {
						nx = px[p] - 1;
						ny = py[p];
						flag = 1;
				}
			}
			for (int i = 1; i <=2; i++) {
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] - 2] == 'I') { // 5X5에서 1열에 4행5행에 I 존재시 이동
						nx = px[p] - 1;
						ny = py[p] + 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] - 2] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] - 1;
						ny = py[p] + 1;
						flag = 1;
					}
				}
			}
			for (int i = -2; i < 0; i++) { //5X5에서 5열에 1행2행에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] + 2] == 'I') {
						nx = px[p] + 1;
						ny = py[p] - 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] + 2] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] + 1;
						ny = py[p] - 1;
						flag = 1;
					}
				}
			}
			for (int k = 0; k < cnt; k++) {
				if (back_buf[py[p]][px[p] + 2] == 'I') { //5X5에서 5열에 3행에 I 존재시 이동
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
				else if (back_buf[py[p]][px[p] + 2] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = 1; i <= 2; i++) {
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] + 2] == 'I') { //5X5에서 5열에 4행5행에 I 존재시 이동
						nx = px[p] + 1;
						ny = py[p] + 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] + 2] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] + 1;
						ny = py[p] + 1;
						flag = 1;
					}
				}
			}
			for (int i = -1; i <= 1; i++) { // 5X5에서 1행 2,3,4열에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] - 2][px[p] + i] == 'I') {
						nx = px[p];
						ny = py[p] - 1;
						flag = 1;
					}
					else if (back_buf[py[p] - 2][px[p] + i] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p];
						ny = py[p] - 1;
						flag = 1;
					}
				}
			}
			for (int i = -1; i <= 1; i++) { //5X5에서 5행 2,3,4열에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + 2][px[p] + i] == 'I') {
						nx = px[p];
						ny = py[p] + 1;
						flag = 1;
					}
					else if (back_buf[py[p] + 2][px[p] + i] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p];
						ny = py[p] + 1;
						flag = 1;
					}
				}
			}
		}
		if (flag == 0) {
			for (int i = -3; i < 0; i++) { //7X7에서 1열에 1행2행3행에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] - 3] == 'I') {
						nx = px[p] - 1;
						ny = py[p] - 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] - 3] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] - 1;
						ny = py[p] - 1;
						flag = 1;
					}
				}
			}
			for (int k = 0; k < cnt; k++) {
				if (back_buf[py[p]][px[p] - 3] == 'I') { //7X7에서 1열에 4행에 I 존재시 이동
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
				else if (back_buf[py[p]][px[p] - 3] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = 1; i <= 3; i++) {
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] - 3] == 'I') { // 7X7에서 1열에 5행6행7행에 I 존재시 이동
						nx = px[p] - 1;
						ny = py[p] + 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] - 3] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] - 1;
						ny = py[p] + 1;
						flag = 1;
					}
				}
			}
			for (int i = -3; i < 0; i++) { //7X7에서 7열에 1행2행3행에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] + 3] == 'I') {
						nx = px[p] + 1;
						ny = py[p] - 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] + 3] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] + 1;
						ny = py[p] - 1;
						flag = 1;
					}
				}
			}
			for (int k = 0; k < cnt; k++) {
				if (back_buf[py[p]][px[p] + 3] == 'I') { //7X7에서 7열에 4행에 I 존재시 이동
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
				else if (back_buf[py[p]][px[p] + 3] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = 1; i <= 3; i++) {
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] + 3] == 'I') { //7X7에서 7열에 5행6행7행에 I 존재시 이동
						nx = px[p] + 1;
						ny = py[p] + 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] + 3] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] + 1;
						ny = py[p] + 1;
						flag = 1;
					}
				}
			}
			for (int i = -2; i <= 2; i++) { // 7X7에서 1행 2,3,4,5,6열에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] - 3][px[p] + i] == 'I') {
						nx = px[p];
						ny = py[p] - 1;
						flag = 1;
					}
					else if (back_buf[py[p] - 3][px[p] + i] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p];
						ny = py[p] - 1;
						flag = 1;
					}
				}
			}
			for (int i = -2; i <= 2; i++) { //7X7에서 5행 2,3,4,5,6열에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + 3][px[p] + i] == 'I') {
						nx = px[p];
						ny = py[p] + 1;
						flag = 1;
					}
					else if (back_buf[py[p] + 3][px[p] + i] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p];
						ny = py[p] + 1;
						flag = 1;
					}
				}
			}
		}
		if (flag == 0) {
			for (int i = -4; i < 0; i++) { //9X9에서 1열에 1행2행3행4행에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] - 4] == 'I') {
						nx = px[p] - 1;
						ny = py[p] - 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] - 4] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] - 1;
						ny = py[p] - 1;
						flag = 1;
					}
				}
			}
			for (int k = 0; k < cnt; k++) {
				if (back_buf[py[p]][px[p] - 4] == 'I') { //9X9에서 1열에 5행에 I 존재시 이동
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
				else if (back_buf[py[p]][px[p] - 4] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = 1; i <= 4; i++) {
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] - 4] == 'I') { // 9X9에서 1열에 6행7행8행9행에 I 존재시 이동
						nx = px[p] - 1;
						ny = py[p] + 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] - 4] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] - 1;
						ny = py[p] + 1;
						flag = 1;
					}
				}
			}
			for (int i = -4; i < 0; i++) { //9X9에서 9열에 1행2행3행4행에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] + 4] == 'I') {
						nx = px[p] + 1;
						ny = py[p] - 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] + 4] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] + 1;
						ny = py[p] - 1;
						flag = 1;
					}
				}
			}
			for (int k = 0; k < cnt; k++) {
				if (back_buf[py[p]][px[p] + 4] == 'I') { //9X9에서 9열에 5행에 I 존재시 이동
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
				else if (back_buf[py[p]][px[p] + 4] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = 1; i <= 4; i++) {
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] + 4] == 'I') { //9X9에서 9열에 6행7행8행9행에 I 존재시 이동
						nx = px[p] + 1;
						ny = py[p] + 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] + 4] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] + 1;
						ny = py[p] + 1;
						flag = 1;
					}
				}
			}
			for (int i = -3; i <= 3; i++) { // 9X9에서 1행 2,3,4,5,6,7,8열에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] - 4][px[p] + i] == 'I') {
						nx = px[p];
						ny = py[p] - 1;
						flag = 1;
					}
					else if (back_buf[py[p] - 4][px[p] + i] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p];
						ny = py[p] - 1;
						flag = 1;
					}
				}
			}
			for (int i = -3; i <= 3; i++) { //9X9에서 9행 2,3,4,5,6,7,8열에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + 4][px[p] + i] == 'I') {
						nx = px[p];
						ny = py[p] + 1;
						flag = 1;
					}
					else if (back_buf[py[p] + 4][px[p] + i] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p];
						ny = py[p] + 1;
						flag = 1;
					}
				}
			}
		}
		if (flag == 0) {
			for (int i = -5; i < 0; i++) { //11X11에서 1열에 1행2행3행4행5행에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] - 5] == 'I') {
						nx = px[p] - 1;
						ny = py[p] - 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] - 5] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] - 1;
						ny = py[p] - 1;
						flag = 1;
					}
				}
			}
			for (int k = 0; k < cnt; k++) {
				if (back_buf[py[p]][px[p] - 5] == 'I') { //11X11에서 1열에 6행에 I 존재시 이동
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
				else if (back_buf[py[p]][px[p] - 5] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = 1; i <= 5; i++) {
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] - 5] == 'I') { // 11X11에서 1열에 7행8행9행10행11행에 I 존재시 이동
						nx = px[p] - 1;
						ny = py[p] + 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] - 5] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] - 1;
						ny = py[p] + 1;
						flag = 1;
					}
				}
			}
			for (int i = -5; i < 0; i++) { //11X11에서 11열에 1행2행3행4행5행에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] + 5] == 'I') {
						nx = px[p] + 1;
						ny = py[p] - 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] + 5] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] + 1;
						ny = py[p] - 1;
						flag = 1;
					}
				}
			}
			for (int k = 0; k < cnt; k++) {
				if (back_buf[py[p]][px[p] + 5] == 'I') { //11X11에서 11열에 6행에 I 존재시 이동
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
				else if (back_buf[py[p]][px[p] + 5] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = 1; i <= 5; i++) {
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + i][px[p] + 5] == 'I') { //11X11에서 11열에 7행8행9행10행11행에 I 존재시 이동
						nx = px[p] + 1;
						ny = py[p] + 1;
						flag = 1;
					}
					else if (back_buf[py[p] + i][px[p] + 5] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p] + 1;
						ny = py[p] + 1;
						flag = 1;
					}
				}
			}
			for (int i = -4; i <= 4; i++) { // 11X11에서 1행 2,3,4,5,6,7,8,9,10열에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] - 5][px[p] + i] == 'I') {
						nx = px[p];
						ny = py[p] - 1;
						flag = 1;
					}
					else if (back_buf[py[p] - 5][px[p] + i] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p];
						ny = py[p] - 1;
						flag = 1;
					}
				}
			}
			for (int i = -4; i <= 4; i++) { //11X11에서 11행 2,3,4,5,6,7,8,9,10열에 I 존재시 이동
				for (int k = 0; k < cnt; k++) {
					if (back_buf[py[p] + 5][px[p] + i] == 'I') {
						nx = px[p];
						ny = py[p] + 1;
						flag = 1;
					}
					else if (back_buf[py[p] + 5][px[p] + i] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
						nx = px[p];
						ny = py[p] + 1;
						flag = 1;
					}
				}
			}
		}
		if (flag == 0) {//왼 or 오른쪽 6~20칸에 I 존재시 이동
			for (int j = 6; j <= 20; j++) {
				for (int i = -5; i <= 5; i++) {
					for (int k = 0; k < cnt; k++) {
						if (back_buf[py[p] + i][px[p] + j] == 'I') {
							nx = px[p] + 1;
							ny = py[p];
							flag = 1;
							break;
						}
						else if (back_buf[py[p] + i][px[p] + j] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
							nx = px[p] + 1;
							ny = py[p];
							flag = 1;
							break;
						}
					}
					if (flag == 1) { break; }
				}
				if (flag == 1) { break; }
				for (int i = -5; i <= 5; i++) {
					for (int k = 0; k < cnt; k++) {
						if (back_buf[py[p] + i][px[p] - j] == 'I') {
							nx = px[p] - 1;
							ny = py[p];
							flag = 1;
							break;
						}
						else if (back_buf[py[p] + i][px[p] - j] == '0' + survive_p[k] && player[survive_p[k]].hasitem == true && flag == 0) {
							nx = px[p] - 1;
							ny = py[p];
							flag = 1;
							break;
						}
					}
					if (flag == 1) { break; }
				}
				if (flag == 1) { break; }
			}
		}

		if(flag==0){
			nx = px[p] + randint(-1, 1);
			ny = py[p] + randint(-1, 1);
		}

		for (int i = 0; i < n_player; i++) { // 가장 가까운 경로에 사람 있으면 멈춤
			if (back_buf[ny][nx] == '0' + i|| back_buf[ny][nx] == 'I') {
				someone = 1;
				break;
			}
		}
		if (someone == 1) { break; }

	} while (!placable(nx, ny));
	if (someone == 0) {
		move_record(p, nx, ny);
	}
}

void get_message(int p) {
	display();
	gotoxy(0, N_ROW + 1);
	printf("%d 플레이어가 '%s'(을)를 획득했습니다.", p, player[p].item.name);
	Sleep(1000);
	gotoxy(0, N_ROW + 1);
	printf("                                                         ");
}

void change_message(int p) {
	display();
	gotoxy(0, N_ROW + 1);
	printf("%d 플레이어가 '%s'(으)로 아이템을 교환했습니다.",p,player[p].item.name);
	Sleep(1000);
	gotoxy(0, N_ROW + 1);
	printf("                                                             ");
}

void get_item(int p) {
	for (int i = -1; i <= 1; i += 2) { // 위 아래
		if (back_buf[py[p] + i][px[p]] == 'I'&& player[p].hasitem==false) {
			back_buf[py[p] + i][px[p]] = ' ';
			player[p].hasitem = true;
			for (int j = 0; j < item_cnt;j++) {
				if (py[p]+i==iy[j]&&px[p]==ix[j]) {
					player[p].item = night_item[j];
					player[p].stamina += player[p].item.stamina_buf;
					if (player[p].stamina > 100) { player[p].stamina = 100;}
					get_message(p);
				}
			}
		}
	}
	for (int i = -1; i <= 1; i += 2) { // 좌 우
		if (back_buf[py[p]][px[p]+i] == 'I' && player[p].hasitem == false) {
			back_buf[py[p]][px[p]+i] = ' ';
			player[p].hasitem = true;
			for (int j = 0; j < item_cnt; j++) {
				if (py[p] == iy[j] && px[p] + i == ix[j]) {
					player[p].item = night_item[j];
					player[p].stamina += player[p].item.stamina_buf;
					if (player[p].stamina > 100) { player[p].stamina = 100; }
					get_message(p);
				}
			}
		}
	}
}

void first_lucky(void) {
	for (int i = 0; i < cnt; i++) {
		get_item(survive_p[i]);
	}
}

void change_item_0(void) {
	display();
	ITEM temp[ITEM_MAX];
	for (int i = -1; i <= 1; i += 2) { // 위 아래
		if (back_buf[py[0] + i][px[0]] == 'I' && player[0].hasitem == true) {
			int choose;
			gotoxy(25, 4);
			printf("아이템을 발견했습니다.");
			gotoxy(25, 5);
			printf("아이템을 바꾸시려면 1, 무시하려면 2를 눌러주세요 (1/2) :");
			scanf_s("%d", &choose);
			if (choose == 1) {
				for (int j = 0; j < item_cnt; j++) {
					if (py[0] + i == iy[j] && px[0] == ix[j]) {
						temp[0] = player[0].item;
						player[0].item = night_item[j];
						night_item[j] = temp[0];
						player[0].stamina += player[0].item.stamina_buf;
						if (player[0].stamina > 100) { player[0].stamina = 100; }
						change_message(0);
					}
				}
			}
			gotoxy(25, 4);
			printf("                        ");
			gotoxy(25, 5);
			printf("                                                              ");
		}
	}
	for (int i = -1; i <= 1; i += 2) { // 좌 우
		if (back_buf[py[0]][px[0]+i] == 'I' && player[0].hasitem == true) {
			int choose;
			gotoxy(25, 4);
			printf("아이템을 발견했습니다.");
			gotoxy(25, 5);
			printf("아이템을 바꾸시려면 1, 무시하려면 2를 눌러주세요 (1/2) :");
			scanf_s("%d", &choose);
			if (choose == 1) {
				for (int j = 0; j < item_cnt; j++) {
					if (py[0] == iy[j] && px[0] + i == ix[j]) {
						temp[0] = player[0].item;
						player[0].item = night_item[j];
						night_item[j] = temp[0];
						player[0].stamina += player[0].item.stamina_buf;
						if (player[0].stamina > 100) { player[0].stamina = 100; }
						change_message(0);
					}
				}
			}
			gotoxy(25, 4);
			printf("                        ");
			gotoxy(25, 5);
			printf("                                                              ");
		}
	}
}
void change_item(int p) {
	ITEM temp[ITEM_MAX];
	for (int i = -1; i <= 1; i += 2) { // 위 아래
		if (back_buf[py[p] + i][px[p]] == 'I' && player[0].hasitem == true) {
			int g_or_not = randint(CHA,IGN);
			if (g_or_not == CHA) {
				for (int j = 0; j < item_cnt; j++) {
					if (py[p] + i== iy[j] && px[p] == ix[j]) {
						temp[p] = player[p].item;
						player[p].item = night_item[j];
						night_item[j] = temp[p];
						player[p].stamina += player[p].item.stamina_buf;
						if (player[p].stamina > 100) { player[p].stamina = 100; }
						change_message(0);
					}
				}
			}
		}
	}
	for (int i = -1; i <= 1; i += 2) { // 좌 우
		if (back_buf[py[p]][px[p] + i] == 'I' && player[0].hasitem == true) {
			int g_or_not = randint(CHA, IGN);
			if (g_or_not == CHA) {
				for (int j = 0; j < item_cnt; j++) {
					if (py[p] == iy[j] && px[p] + i == ix[j]) {
						temp[p] = player[p].item;
						player[p].item = night_item[j];
						night_item[j] = temp[p];
						player[p].stamina += player[p].item.stamina_buf;
						if (player[p].stamina > 100) { player[p].stamina = 100; }
						change_message(0);
					}
				}
			}
		}
	}
}

void pto0(void) {
	display();
	if (player[0].stamina == 0) { return 0; } //스태미나 0이면 상호작용 불가
	int choose;
	ITEM temp[ITEM_MAX];
	ITEM nothing[1] = { 0 }; //강탈 시 0으로 덮어줄 변수
	for (int i = 0; i < cnt; i++) {
		if (back_buf[py[0] + 1][px[0]] == '0' + survive_p[i] || back_buf[py[0] - 1][px[0]] == '0' + survive_p[i] || back_buf[py[0]][px[0] + 1] == '0' + survive_p[i] || back_buf[py[0]][px[0] - 1] == '0' + survive_p[i]) {
			gotoxy(25, 4);
			printf("다른 플레이어와 만났습니다!");
			gotoxy(25, 5);
			printf("원하는 선택지를 고르세요.");
			gotoxy(25, 6);
			printf("1)강탈시도   2)회유시도   3)무시 (1/2/3) :");
			scanf_s("%d", &choose);
			if (choose == STR) { //강탈 시도
				double real_str_0 = (player[0].str + player[0].item.str_buf) * (player[0].stamina/100.0);
				double real_str_p = (player[survive_p[i]].str + player[survive_p[i]].item.str_buf) * (player[survive_p[i]].stamina/100.0);
				if (real_str_0 > real_str_p) { //성공
					if (player[0].hasitem == true) { //0이 아이템이 있으면
						temp[0] = player[0].item;
						player[0].item = player[i].item;
						player[i].item = temp[0];
					}
					else{ // 0이 아이템이 없으면
						player[0].item = player[i].item;
						player[i].item = nothing[0];
					}
					player[0].stamina -= 40; //스태미나 소모
					if (player[0].stamina < 0) {player[0].stamina = 0;}
				}
				else { //실패
					player[0].stamina -= 60;
				}
			}
			else if (choose == INT) {

			}
			else {

			}
		}
	}
}

void nightgame(void) {
	nightgame_init();
	left_player();
	system("cls");
	display();
	first_lucky(); //처음 랜덤 배정에서 아이템이 바로 옆에 있으면 get
	while (1) {
		// player 0만 손으로 움직임(4방향)
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			move_0_manual(key);
			if (survive_p[0] == 0) { // 무궁화에서 0이 죽었을 때 생각해서 살았을 때만 
				get_item(survive_p[0]);
				change_item_0();
			}
		}
		// player 1 부터는 랜덤으로 움직임(8방향)
		for (int i = 0; i < cnt; i++) {
			if (tick % period[survive_p[i]] == 0&&survive_p[i]!=0) {
				move_rand(survive_p[i], -1);
				get_item(survive_p[i]);
				change_item(survive_p[i]);
			}
		}
		display();
		Sleep(10);
		tick += 10;
	}
}