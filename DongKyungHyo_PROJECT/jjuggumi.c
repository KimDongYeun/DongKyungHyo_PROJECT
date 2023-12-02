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
	printf("    | |    | || | | || |  \\/| |  \\/| | | || .  . |  | |   | |  \\/  /_\\ \\| .  . || |__\n");
	printf("    | |    | || | | || | __ | | __ | | | || |\\/| |  | |   | | __ |  _  || |\\/| ||  __|\n");
	printf("/\\__/ //\\__/ /| |_| || |_/ /| |_/ /| |_| || |  | | _| |_  | |_/ /| | | || |  | || |___\n");
	printf("\\____/ \\____/  \\___/  \\____/ \\____/ \\___/ \\_|  |_/ \\___/   \\____/\\_| |_/\\_|  |_/\\____/\n\n");
	printf("======================================================\n"); Sleep(100);
}

int jjuggumi_init(void) {
	srand((unsigned int)time(NULL));

	FILE* fp;
	fopen_s(&fp, DATA_FILE, "r");
	if (fp == NULL) {
		return -1;	// -1 리턴하면 메인함수에서 처리하고 종료
	}

	//플레이어 데이터 load
	fscanf_s(fp, "%d", &n_player);
	for (int i = 0; i < n_player; i++) {
		// 아직 안 배운 문법(구조체 포인터, 간접참조연산자)
		PLAYER* p = &player[i];

		// 파일에서 각 스탯 최댓값 읽기
		fscanf_s(fp, "%s%d%d",
						p->name, (unsigned int)sizeof(p->name),
						&(p->intel), &(p->str));
			p->stamina = 100; // 100%

			//현재 상태
			p->is_alive = true;
			p->hasitem = false;
	}

	// 아이템 데이터 load
	fscanf_s(fp, "%d", &n_item);
	for (int i = 0; i < n_item; i++) {
		fscanf_s(fp, "%s%d%d%d",
						item[i].name, (unsigned int)sizeof(item[i].name),
						&(item[i].intel_buf),
						&(item[i].str_buf),
						&(item[i].stamina_buf));
	}

	fclose(fp);
	n_alive = n_player;
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
	mugunghwa();
	nightgame();
	juldarigi();
	jebi();
	ending();
	return 0;
}
