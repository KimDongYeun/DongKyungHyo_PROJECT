// 2023-2 고급프로그래밍 과제: 쭈꾸미 게임
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>
#include <time.h>
#include "jjuggumi.h"
#include "canvas.h"

#define	DATA_FILE	"jjuggumi.dat"

int jjuggumi_init(void);
void ascii_art(void);

// low 이상 high 이하 난수를 발생시키는 함수
int randint(int low, int high) {
	int rnum = rand() % (high - low + 1) + low;
	return rnum;
}

void ascii_art(void) {
	system("cls");
	printf("======================================================\n\n"); Sleep(100);
	printf("               (:◎)≡ 쭈꾸미 게임 ≡(◎:)\n\n"); Sleep(100);
	printf("======================================================\n"); Sleep(100);
	printf("   ___    ___  _   _  _____  _____  _   _ ___  ___ _____   _____   ___  ___  ___ _____\n");
	printf("  |_  |  |_  || | | ||  __ \\|  __ \\| | | ||  \\/  ||_   _| |  __ \\ / _ \\ |  \\/  ||  ___|\n");
	printf("    | |    | || | | || |  \\/| |  \\/| | | || .  . |  | |   | |  \\/ /_\\ \\| .  . || |__\n");
	printf("    | |    | || | | || | __ | | __ | | | || |\\/| |  | |   | | __ |  _  || |\\/| ||  __|\n");
	printf("/\\__/ //\\__/ /| |_| || |_/ /| |_/ /| |_| || |  | | _| |_  | |_/ /| | | || |  | || |___\n");
	printf("\\____/ \\____/  \\___/  \\____/ \\____/ \\___/ \\_|  |_/ \\___/   \\____/\\_| |_/\\_|  |_/\\____/\n\n");
	printf("======================================================\n"); Sleep(100);
}

int jjuggumi_init(void) {
	srand((unsigned int)time(NULL));

	printf("플레이어 수: ");
	scanf_s("%d", &n_player);

	n_alive = n_player;
	for (int i = 0; i < n_player; i++) {
		player[i] = true;
	}
	return 0;
}

int intro(void) {
	ascii_art(); //인트로 
	printf("\n게임을 시작합니다!\n"); Sleep(3000); //3초 후에 게임시작
	system("cls");
	return 0;
}


int main(void) {
	jjuggumi_init();
	intro();
	//mugunghwa();
	ending();
	return 0;
}
