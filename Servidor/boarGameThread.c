#include "servidor.h"
DWORD WINAPI ThreadsFaixa(LPVOID param) {
	Info* pData = (Info*)param;
	COORD pos;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	WaitForSingleObject(pData->hTimer, INFINITE);
	pos.X = 0;
	pos.Y = pData->nFaixaResp;

	int xtam[3];
	TCHAR tam[20];
	int i = 0, j = 0;
	/*
	for ( i = 0; i < 3; i++) {
		int n = (rand() % 20) + 1;
		do {
			for (j = 0; j < 3; j++) {
				if (xtam[j] == n) {
					j--;
					break;
				}
			}
		} while (j != 2);
		tam[n] = pData->o.c;
	}
	int flag = 20;
	*/
	do {
		WaitForSingleObject(pData->hMutexArray, INFINITE);
		GetConsoleScreenBufferInfo(pData->hStdout, &csbi);
		SetConsoleCursorPosition(pData->hStdout, pos);
		SetConsoleTextAttribute(pData->hStdout, FOREGROUND_BLUE << pData->id);
		_tprintf_s(TEXT("C"));
		pos.X++;
		SetConsoleTextAttribute(pData->hStdout, csbi.wAttributes);
		SetConsoleCursorPosition(pData->hStdout, csbi.dwCursorPosition);
		ReleaseMutex(pData->hMutexArray);
		Sleep(pData->veloc);
	} while (pData->end);
	WaitForSingleObject(pData->hMutexArray, INFINITE);
	ExitThread(1);
}
void lancaThread(FaixaVelocity dados) {
	COORD posI = {0,0};
	DWORD res;

	TCHAR(*boardGameArray)[columns] = (TCHAR(*)[columns])malloc(dados.faixa * columns * sizeof(TCHAR));
	if (boardGameArray == NULL) {
		_tprintf_s(TEXT("Erro a criar memória para o jogo\n"));
		ExitProcess(-1);
	}
	HANDLE hMutexArray = CreateMutex(NULL, FALSE, NULL);
	HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE* hThreads = (HANDLE*)malloc(dados.faixa * sizeof(HANDLE));
	BOOL succeed = FillConsoleOutputCharacter(hStdout, _T(' '), JANELAX * JANELAY, posI, &res);

	if (hMutexArray == NULL || hTimer == NULL || !succeed || hThreads ==NULL) {
		_tprintf_s(TEXT("Erro a criar condições para o jogo."));
		ExitProcess(1);
		//Criar funcao para encerrar tudo
	}

	posI.X = 0;
	posI.Y = 0;

	SetConsoleCursorPosition(hStdout, posI);
	
	Info* send = (Info*)malloc(dados.faixa * sizeof(Info));
	
	if (send == NULL) {
		printf_s(TEXT("Erro a criar condições para o jogo."));
		ExitProcess(1);
	}

	objs o;
	o.c = TEXT('C');
	o.o = TEXT('O');
	o.s = TEXT('S');
	
	//janela 60x40 logo guardar as ultimas 4 linhas para admin escrever 
	for (int i = 0; i < 3; i++) {
		send[i].id = i;
		send[i].nFaixaResp = JANELAY - dados.faixa - i - 1;
		send[i].arrayGame = boardGameArray;
		send[i].o = o;
		send[i].veloc = dados.faixa;
		send[i].nFaixaResp = 0;
		send[i].hMutexArray = hMutexArray;
		send[i].hTimer = hTimer;
		send[i].hStdout = hStdout;
		send[i].end = 1;
		hThreads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadsFaixa, (LPVOID)&send[i], 0, & send[i].id);
		if (hThreads[i] == NULL)
		{
			_tprintf_s(TEXT("Não foi possível lançar as threads para o jogo iniciar.\n"));
			ExitProcess(-1);
		}
	}
	LARGE_INTEGER liArranca;
	liArranca.QuadPart = -5 * 10000000;
	SetWaitableTimer(hTimer, (LARGE_INTEGER*)&liArranca, 0, NULL, NULL, FALSE);


}