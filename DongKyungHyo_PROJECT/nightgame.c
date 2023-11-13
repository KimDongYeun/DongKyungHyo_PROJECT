#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>
#include <time.h>


#define DIR_UP		2
#define DIR_DOWN	3
#define DIR_LEFT	1
#define DIR_RIGHT	0
int cnt = 0;
int survive_p[PLAYER_MAX];

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
		// ���� �ڸ��� ������ �ٽ� ����
		do {
			y = randint(1, N_ROW - 2);
			x = randint(1, N_COL - 2);
		} while (!placable(x, y));
		px[survive_p[i]] = x;
		py[survive_p[i]] = y;
		period[survive_p[i]] = randint(10, 30);

		back_buf[py[survive_p[i]]][px[survive_p[i]]] = '0' + survive_p[i];  // (0 .. n_player-1)
	}
	int item_cnt = randint(1, 4);
	ITEM night_item[ITEM_MAX];
	for (int i = 0; i < item_cnt; i++) {
		do {
			y = randint(1, N_ROW - 2);
			x = randint(1, N_COL - 2);
		} while (!placable(x, y));
		int item_num = randint(0, ITEM_MAX-1);
		night_item[i] = item[item_num];
		ix[i] = x;
		iy[i] = y;

		back_buf[iy[i]][ix[i]] = 'I';
	}
	

	tick = 0;
}

// back_buf[][]�� ���
void move_record(int player, int nx, int ny) {
	int p = player;  // �̸��� ��...
	back_buf[ny][nx] = back_buf[py[p]][px[p]];
	back_buf[py[p]][px[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}

void move_0_manual(key_t key) {
	// �� �������� ������ �� x, y�� delta
	static int dx[4] = { 1, -1, 0, 0 };
	static int dy[4] = { 0, 0, -1, 1 };

	int dir;  // ������ ����(0~3)
	switch (key) {
	case K_UP: dir = DIR_UP; break;
	case K_DOWN: dir = DIR_DOWN; break;
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;
	default: return;
	}

	// �������� ���� �ڸ�
	int nx, ny;
	nx = px[0] + dx[dir];
	ny = py[0] + dy[dir];
	if (!placable(nx, ny)) {
		return;
	}

	move_record(0, nx, ny);
}

void move_rand(int player, int dir) {
	int p = player;  // �̸��� ��...
	int nx, ny;  // �������� ������ ���� �ڸ�
	int flag = 0;
	int someone = 0;
	// ������ ������ ���� ���� ���ٰ� ����(���� ������ ����)	

	do { 
		for (int j = -1; j <= 1; j += 2) { // 3X3���� �밢���� I ���� �� ��ó�� �̵�
			for (int i = -1; i <= 1; i += 2) {
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
			}
		}
		if (flag == 0) {
			for (int i = -2; i <0; i++) { //5X5���� 1���� 1��2�࿡ I ����� �̵�
				if (back_buf[py[p] + i][px[p] -2] == 'I') {
					nx = px[p] - 1;
					ny = py[p] - 1;
					flag = 1;
				}
			}
			if (back_buf[py[p]][px[p] - 2] == 'I') { //5X5���� 1���� 3�࿡ I ����� �̵�
				nx = px[p] - 1;
				ny = py[p];
				flag = 1;
			}
			for (int i = 1; i <=2; i++) {
				if (back_buf[py[p] + i][px[p] - 2] == 'I') { // 5X5���� 1���� 4��5�࿡ I ����� �̵�
					nx = px[p] - 1;
					ny = py[p] + 1;
					flag = 1;
				}
			}
			for (int i = -2; i < 0; i++) { //5X5���� 5���� 1��2�࿡ I ����� �̵�
				if (back_buf[py[p] + i][px[p] + 2] == 'I') {
					nx = px[p] + 1;
					ny = py[p] - 1;
					flag = 1;
				}
			}
			if (back_buf[py[p]][px[p] + 2] == 'I') { //5X5���� 5���� 3�࿡ I ����� �̵�
				nx = px[p] + 1;
				ny = py[p];
				flag = 1;
			}
			for (int i = 1; i <= 2; i++) {
				if (back_buf[py[p] + i][px[p] + 2] == 'I') { //5X5���� 5���� 4��5�࿡ I ����� �̵�
					nx = px[p] + 1;
					ny = py[p] + 1;
					flag = 1;
				}
			}
			for (int i = -1; i <= 1; i++) { // 5X5���� 1�� 2,3,4���� I ����� �̵�
				if (back_buf[py[p] - 2][px[p] + i] == 'I') {
					nx = px[p];
					ny = py[p] - 1;
					flag = 1;
				}
			}
			for (int i = -1; i <= 1; i++) { //5X5���� 5�� 2,3,4���� I ����� �̵�
				if (back_buf[py[p] + 2][px[p] + i] == 'I') {
					nx = px[p];
					ny = py[p] + 1;
					flag = 1;
				}
			}
		}
		if (flag == 0) {
			for (int i = -3; i < 0; i++) { //7X7���� 1���� 1��2��3�࿡ I ����� �̵�
				if (back_buf[py[p] + i][px[p] - 3] == 'I') {
					nx = px[p] - 1;
					ny = py[p] - 1;
					flag = 1;
				}
			}
			if (back_buf[py[p]][px[p] - 3] == 'I') { //7X7���� 1���� 4�࿡ I ����� �̵�
				nx = px[p] - 1;
				ny = py[p];
				flag = 1;
			}
			for (int i = 1; i <= 3; i++) {
				if (back_buf[py[p] + i][px[p] - 3] == 'I') { // 7X7���� 1���� 5��6��7�࿡ I ����� �̵�
					nx = px[p] - 1;
					ny = py[p] + 1;
					flag = 1;
				}
			}
			for (int i = -3; i < 0; i++) { //7X7���� 7���� 1��2��3�࿡ I ����� �̵�
				if (back_buf[py[p] + i][px[p] + 3] == 'I') {
					nx = px[p] + 1;
					ny = py[p] - 1;
					flag = 1;
				}
			}
			if (back_buf[py[p]][px[p] + 3] == 'I') { //7X7���� 7���� 4�࿡ I ����� �̵�
				nx = px[p] + 1;
				ny = py[p];
				flag = 1;
			}
			for (int i = 1; i <= 3; i++) {
				if (back_buf[py[p] + i][px[p] + 3] == 'I') { //7X7���� 7���� 5��6��7�࿡ I ����� �̵�
					nx = px[p] + 1;
					ny = py[p] + 1;
					flag = 1;
				}
			}
			for (int i = -2; i <= 2; i++) { // 7X7���� 1�� 2,3,4,5,6���� I ����� �̵�
				if (back_buf[py[p] - 3][px[p] + i] == 'I') {
					nx = px[p];
					ny = py[p] - 1;
					flag = 1;
				}
			}
			for (int i = -2; i <= 2; i++) { //7X7���� 5�� 2,3,4,5,6���� I ����� �̵�
				if (back_buf[py[p] + 3][px[p] + i] == 'I') {
					nx = px[p];
					ny = py[p] + 1;
					flag = 1;
				}
			}
		}
		if (flag == 0) {
			for (int i = -4; i < 0; i++) { //9X9���� 1���� 1��2��3��4�࿡ I ����� �̵�
				if (back_buf[py[p] + i][px[p] - 4] == 'I') {
					nx = px[p] - 1;
					ny = py[p] - 1;
					flag = 1;
				}
			}
			if (back_buf[py[p]][px[p] - 4] == 'I') { //9X9���� 1���� 5�࿡ I ����� �̵�
				nx = px[p] - 1;
				ny = py[p];
				flag = 1;
			}
			for (int i = 1; i <= 4; i++) {
				if (back_buf[py[p] + i][px[p] - 4] == 'I') { // 9X9���� 1���� 6��7��8��9�࿡ I ����� �̵�
					nx = px[p] - 1;
					ny = py[p] + 1;
					flag = 1;
				}
			}
			for (int i = -4; i < 0; i++) { //9X9���� 9���� 1��2��3��4�࿡ I ����� �̵�
				if (back_buf[py[p] + i][px[p] + 4] == 'I') {
					nx = px[p] + 1;
					ny = py[p] - 1;
					flag = 1;
				}
			}
			if (back_buf[py[p]][px[p] + 4] == 'I') { //9X9���� 9���� 5�࿡ I ����� �̵�
				nx = px[p] + 1;
				ny = py[p];
				flag = 1;
			}
			for (int i = 1; i <= 4; i++) {
				if (back_buf[py[p] + i][px[p] + 4] == 'I') { //9X9���� 9���� 6��7��8��9�࿡ I ����� �̵�
					nx = px[p] + 1;
					ny = py[p] + 1;
					flag = 1;
				}
			}
			for (int i = -3; i <= 3; i++) { // 9X9���� 1�� 2,3,4,5,6,7,8���� I ����� �̵�
				if (back_buf[py[p] - 4][px[p] + i] == 'I') {
					nx = px[p];
					ny = py[p] - 1;
					flag = 1;
				}
			}
			for (int i = -3; i <= 3; i++) { //9X9���� 9�� 2,3,4,5,6,7,8���� I ����� �̵�
				if (back_buf[py[p] + 4][px[p] + i] == 'I') {
					nx = px[p];
					ny = py[p] + 1;
					flag = 1;
				}
			}
		}
		if (flag == 0) {
			for (int i = -5; i < 0; i++) { //11X11���� 1���� 1��2��3��4��5�࿡ I ����� �̵�
				if (back_buf[py[p] + i][px[p] - 5] == 'I') {
					nx = px[p] - 1;
					ny = py[p] - 1;
					flag = 1;
				}
			}
			if (back_buf[py[p]][px[p] - 5] == 'I') { //11X11���� 1���� 6�࿡ I ����� �̵�
				nx = px[p] - 1;
				ny = py[p];
				flag = 1;
			}
			for (int i = 1; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 5] == 'I') { // 11X11���� 1���� 7��8��9��10��11�࿡ I ����� �̵�
					nx = px[p] - 1;
					ny = py[p] + 1;
					flag = 1;
				}
			}
			for (int i = -5; i < 0; i++) { //11X11���� 11���� 1��2��3��4��5�࿡ I ����� �̵�
				if (back_buf[py[p] + i][px[p] + 5] == 'I') {
					nx = px[p] + 1;
					ny = py[p] - 1;
					flag = 1;
				}
			}
			if (back_buf[py[p]][px[p] + 5] == 'I') { //11X11���� 11���� 6�࿡ I ����� �̵�
				nx = px[p] + 1;
				ny = py[p];
				flag = 1;
			}
			for (int i = 1; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 5] == 'I') { //11X11���� 11���� 7��8��9��10��11�࿡ I ����� �̵�
					nx = px[p] + 1;
					ny = py[p] + 1;
					flag = 1;
				}
			}
			for (int i = -4; i <= 4; i++) { // 11X11���� 1�� 2,3,4,5,6,7,8,9,10���� I ����� �̵�
				if (back_buf[py[p] - 5][px[p] + i] == 'I') {
					nx = px[p];
					ny = py[p] - 1;
					flag = 1;
				}
			}
			for (int i = -4; i <= 4; i++) { //11X11���� 11�� 2,3,4,5,6,7,8,9,10���� I ����� �̵�
				if (back_buf[py[p] + 5][px[p] + i] == 'I') {
					nx = px[p];
					ny = py[p] + 1;
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 6ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 6] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 6] == 'I') {
					nx = px[p] -1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 7ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 7] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 7] == 'I') {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 8ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 8] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 8] == 'I') {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 9ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 9] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 9] == 'I') {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 10ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 10] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 10] == 'I') {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 11ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 11] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 11] == 'I') {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 12ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 12] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 12] == 'I') {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 13ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 13] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 13] == 'I') {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 14ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 14] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 14] == 'I') {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 15ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 15] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 15] == 'I') {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 16ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 16] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 16] == 'I') {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 17ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 17] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 17] == 'I') {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 18ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 18] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 18] == 'I') {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 19ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 19] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 19] == 'I') {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if (flag == 0) {//�� or ������ 20ĭ�� I ����� �̵�
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] + 20] == 'I') {
					nx = px[p] + 1;
					ny = py[p];
					flag = 1;
				}
			}
			for (int i = -5; i <= 5; i++) {
				if (back_buf[py[p] + i][px[p] - 20] == 'I') {
					nx = px[p] - 1;
					ny = py[p];
					flag = 1;
				}
			}
		}
		if(flag==0){
			nx = px[p] + randint(-1, 1);
			ny = py[p] + randint(-1, 1);
		}

		for (int i = 0; i < n_player; i++) { // ���� ����� ��ο� ��� ������ ����
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

void nightgame(void) {
	nightgame_init();
	left_player();
	system("cls");
	display();
	while (1) {
		// player 0�� ������ ������(4����)
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			move_0_manual(key);
		}

		// player 1 ���ʹ� �������� ������(8����)
		for (int i = 0; i < cnt; i++) {
			if (tick % period[survive_p[i]] == 0&&survive_p[i]!=0) {
				move_rand(survive_p[i], -1);
			}
		}

		display();
		Sleep(10);
		tick += 10;
	}
}