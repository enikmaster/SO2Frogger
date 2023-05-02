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
	do {
		WaitForSingleObject(pData->hMutexArray, INFINITE);
		GetConsoleScreenBufferInfo(pData->hStdout, &csbi);
		SetConsoleCursorPosition(pData->hStdout, pos);
		SetConsoleTextAttribute(pData->hStdout, FOREGROUND_BLUE << pData->id);
		for (i = 0; i < columns; i++) {
			if (lstrcmpW(pData->arrayGame[pos.X][pos.Y], TEXT("C")) == 0) {
				pData->arrayGame[pos.X][pos.Y] = TEXT("");
				pData->arrayGame[pos.X + 1][pos.Y] = pData->o.c;
			}
		}
		_tprintf_s(TEXT("%c"), pData->arrayGame[pos.X][pos.Y]);
		if(pos.X == 19)
			pos.X = 0;
		pos.X++;
		SetConsoleTextAttribute(pData->hStdout, csbi.wAttributes);
		SetConsoleCursorPosition(pData->hStdout, csbi.dwCursorPosition);
		ReleaseMutex(pData->hMutexArray);
		Sleep(pData->veloc);
	} while (pData->end);
	WaitForSingleObject(pData->hMutexArray, INFINITE);
	ExitThread(1);
}
void lancaThread(FaixaVelocity dados, COORD posI, HANDLE hStdout) {
	TCHAR(*boardGameArray)[columns] = (TCHAR(*)[columns])malloc(dados.faixa * columns * sizeof(TCHAR));
	if (boardGameArray == NULL) {
		_tprintf_s(TEXT("Erro a criar memória para o jogo\n"));
		ExitProcess(-1);
	}
	HANDLE hMutexArray = CreateMutex(NULL, FALSE, NULL);
	HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	HANDLE* hThreads = (HANDLE*)malloc(dados.faixa * sizeof(HANDLE));
	if (hMutexArray == NULL || hTimer == NULL || hThreads ==NULL) {
		_tprintf_s(TEXT("Erro a criar condições para o jogo."));
		ExitProcess(1);
		//Criar funcao para encerrar tudo
	}

	for (int i = 0; i < dados.faixa; i++) {
		for (int j = 0; j < columns; j++) {
			boardGameArray[i][j] = '_';
		}
	}
	posI.X =0;
	posI.Y = 10;

	SetConsoleCursorPosition(hStdout, posI);
	
	Info* send = (Info*)malloc(dados.faixa * sizeof(Info));
	
	if (send == NULL) {
		printf_s(TEXT("Erro a criar condições para o jogo."));
		ExitProcess(1);
	}

	objs o;
	o.c = 'C';
	o.o = 'O';
	o.s = 'S';
	
	//Preencher array randomly 
	
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
	do {
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < dados.faixa; i++) {
				//_tprintf_s(TEXT("%c"), boardGameArray[i][j]);
			}
			//_tprintf_s("\n");
		}
	} while (1);
	SetWaitableTimer(hTimer, (LARGE_INTEGER*)&liArranca, 0, NULL, NULL, FALSE);
	CloseHandle(hMutexArray);
	CloseHandle(hTimer);
	CloseHandle(hThreads);
	return 0;

}