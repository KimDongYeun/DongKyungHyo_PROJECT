#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"

char out_player_jul[19] = { ' ' }; //죽은애 저장할거
int survive_p[PLAYER_MAX];
int out_p = 0; //죽은애
int cnt_t = 0; //생존
double str = 0; //힘
int key; //키 입력
int hx[3], hy[3]; //줄
int p, diee, di, nu = 0;
int nupgi_turns = 0; //눕기 턴
double before_str = 0; //눕기 하기 전에 저장할 힘
int dx, dy, nx, ny, d; //줄, 플레이어 출력 관련
int not_buhwal[19] = { 0 }; //기존에 계속 생존해 있던 플레이어 저장할거
int buhwal[19] = { -1 };
int num_dead_player = 0;
int rt_alive = 0; lt_alive = 0;
int ticktick = 0;
double lt_str = 0, rt_str = 0;
int l_nup = 0;
int r_nup = 0;

void juldarigi_init();
void print_str();
void p_str();
void move_player();
void move_dash();
void player_print();
void dash_print();
void after_nupgi();
void play_juldarigi();
void star_bu();
int juldarigi();

void juldarigi_init(void) { //맵크기 설정, 죽은애 부활, 플레이어 위치 설정, 줄 위치 설정
	map_init(3, 32);
	for (int i = 0; i < n_player; i++) {
		if (player[i].is_alive == true) {
			survive_p[cnt_t] = i;
			cnt_t++;
			not_buhwal[i] = i;
		}
		else if (player[i].is_alive == false) {
			player[i].is_alive = true;
			player[i].hasitem = false;
			buhwal[i] = i;
			survive_p[cnt_t] = i;
			cnt_t++;
			n_alive++;
		}
	}
	int na = n_player / 2;
	rt_alive = na;
	lt_alive = na;
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

void print_str() {
	for (int i = 0; i < N_COL; i++) {
		printxy(' ', i, 5);
	}
	gotoxy(0, 5);
	printf("str :	%.2f\n", str);
}

void p_str() { //str 계산
	lt_str = 0, rt_str = 0;
	for (int i = 0; i < n_player; i++) {
		if (i % 2 == 0 && player[i].is_alive) {
			lt_str = lt_str + ((player[i].str + player[i].item.str_buf) * (player[i].stamina / 100.0));
		}
		else if (i % 2 != 0 && player[i].is_alive) {
			rt_str = rt_str + ((player[i].str + player[i].item.str_buf) * (player[i].stamina / 100.0));
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

void after_nupgi() { //눕기 한 후 초기화시키기
	for (int i = 0; i < COL_MAX; i++) {
		printxy(' ', i, 4);
	}
		if (nu == 1) {
			for (int i = 0; i < n_player; i += 2) {
				player[i].stamina -= 30;
				if (player[i].stamina < 0) {
					player[i].stamina = 0;
				}
			}
		}
		else if (nu == 2) {
			for (int i = 1; i < n_player; i+=2) {
				player[i].stamina -= 30;
				if (player[i].stamina < 0) {
					player[i].stamina = 0;
				}
			}
		}
	nu = 0;
	nupgi_turns = 0;
	l_nup = 0;
	r_nup = 0;
}

int keytrue = 0;

int turn = 1;
int nup_die = 0;
int nup2_die = 0;
void play_juldarigi() { //게임돌아가는 진짜 코드
	before_str = str;
	int nx, ny;
	if (ticktick % 1000 == 0) {
		for (int i = 0; i < n_player; i++) { //플레이어 움직이게하기, nu, nupgi_turns은 눕기 했을때 2칸 움직이기
			p = i;
			if (str > 0) {
				nx = px[p] + 1;
				ny = py[p];
				if (nu == 2 && nupgi_turns == 1) {
					nx = px[p] + 2;
				}
				else if (nu == 1 && nupgi_turns == 1) {
					nx = px[p] + 1;
				}
				move_player(p, nx, ny);
			}
			else if (str < 0) {
				nx = px[p] - 1;
				ny = py[p];
				if (nu == 1 && nupgi_turns == 1) {
					nx = px[p] - 2;
				}
				else if (nu == 2 && nupgi_turns == 1) {
					nx = px[p] - 1;
				}
				move_player(p, nx, ny);
			}
			else if (str == 0) {
				nx = px[p];
				ny = py[p];
				move_player(p, nx, ny);
			}
			if ((p % 2 == 0 && px[p] >= 15) || (p % 2 != 0 && px[p] <= 15)) { //죽어야 하는 경우
				if (player[p].is_alive) {
					if (p % 2 == 0) { //오른쪽 애 죽었을때
						diee = 1;
					}
					else if (p % 2 != 0) { //왼쪽애 죽었을때
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
						lt_alive -= 1;

					}
					else if (p % 2 != 0 && nu != 0) {
						nup2_die += 1;
						rt_alive -= 1;
					}
				}
			}
		}
		if (diee == 1 || diee == 2) { //죽었을때 상대편 안움직이게 할 코드
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
			if (nup_die != 0 && nu != 0) { //눕기 눌러서 누구 죽었을때 상대방 안움직이게 하기 .눌렀을때
				for (int i = 0; i < n_player; i++) {
					if (nup_die == 1 && lt_alive >= 2) {
						if (i % 2 != 0) {
							nx = px[i];
							ny = 1;
							move_player(i, nx, ny);
						}
					}
					else if ((nup_die == 1 && lt_alive == 0) || nup_die == 2) {
						if (i % 2 != 0) {
							nx = px[i] - 1;
							ny = 1;
							move_player(i, nx, ny);
						}
					}
				}
			}
			else if (nup2_die != 0 && nu != 0) {//눕기 눌러서 누구 죽었을때 상대방 안움직이게 하기 x눌렀을때
				for (int i = 0; i < n_player; i++) {
					if (nup2_die == 1 && rt_alive >= 2) {
						if (i % 2 == 0) {
							nx = px[i];
							ny = 1;
							move_player(i, nx, ny);
						}
					}
					else if ((nup2_die == 1 && rt_alive == 0) || nup2_die == 2) {
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
				nup_die = 0;
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
				for (int j = 0; j < 3; j++) { //죽었을때 줄 안움직이게 하는 코드
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
			}
		}
		else { //죽은애 없을때 줄 움직이게 하는 코드
			int moveAmount = (str > 0) ? 1 : -1;
			for (int j = 0; j < 3; j++) {
				d = j;
				dx = hx[j] + moveAmount;
				dy = hy[j];
				if (nu == 1) {
					if (str > 0) {
						dx = dx;
					}
					else if (str < 0) {
						dx -= 1;
					}
				}
				else if (nu == 2) {
					if (str > 0) {
						dx += 1;
					}
					else if (str < 0) {
						dx = dx;
					}
				}
				if (str == 0) {
					dx = hx[j];
					dy = hy[j];
				}
				move_dash(dx, dy, j);
			}
		}
		dash_print();
		player_print();
		diee = 0;
		nup_die = 0;
		nup2_die = 0;
		if (nu != 0 && nupgi_turns == 1) { //눕기 턴 초기화
			after_nupgi();
			str = before_str;
		}
		p_str(); //str계산하기
		print_str();
	}
}

void star_bu() {
	for (int i = 0; i < n_player; i++) {
		if (i == buhwal[i]) {
			gotoxy(16, 9 + i);
			printf("*");
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
	printf("str :	%.2f", str);
	printf("\n");
	star_bu();
	while (1) {
		if (_kbhit()) {
			int key = _getch();
			if (key == 'q') { break; }
			else if (key == 'x') {
				if (l_nup == 0) {
					int ifff = 0;
					nu = 1;
					gotoxy(0, 4);
					printf("왼쪽 팀이 눕기를 사용했습니다.\n");
					lt_str *= 2;
					str = -lt_str + rt_str;
					print_str();
					nupgi_turns = 1;
					l_nup = 1;
				}
			}
			else if (key == '.') {
				if (r_nup == 0) {
					int ifff = 0;
					nu = 2;
					gotoxy(0, 4);
					printf("오른쪽 팀이 눕기를 사용했습니다.\n");
					rt_str *= 2;
					str = -lt_str + rt_str;
					print_str();
					nupgi_turns = 1;
					r_nup = 1;
				}
			}
			else if (key == 'z') {
				keytrue = 1;
				str -= 1;
				print_str();
			}
			else if (key == '/') {
				keytrue = 1;
				str += 1;
				print_str();
			}
		}
		display();
		star_bu();
		play_juldarigi();
		display();
		star_bu();
		if (di == 1) {
			if (num_dead_player == 1) {
				dialog_juldarigi("player", "dead!", out_player_jul, 1);
			}
			else if (num_dead_player >= 2) {
				dialog_juldarigi("player", "dead!", out_player_jul, num_dead_player);
			}
			di = 0;
			num_dead_player = 0;
		}
		if (keytrue == 1) {
			str = before_str;
			keytrue = 0;
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

		if (allEvenDead || allOddDead) { //처음에 살았었던 애 죽은거 다시 살리고, 아이템 없애고 힘,지능 절반 하락 후 줄다리기 종료
			for (int i = 0; i < n_player; i++) {
				if (player[i].is_alive == false) {
					if (i == not_buhwal[i]) {
						player[i].is_alive = true;
						player[i].str = player[i].str / 2;
						player[i].intel = player[i].intel / 2;
						player[i].hasitem = false;
					}
				}
				/*if (player[i].stamina < 100) {
					player[i].stamina= player[i].stamina+ 40;
					if (player[i].stamina >= 100) {
						player[i].stamina = 100;
					}
				}*/
			}
			break;
		}
		Sleep(10);
		ticktick += 10;
	}
	return 0;
}