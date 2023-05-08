﻿#include "servidor.h"

DWORD WINAPI ThreadsFaixa(LPVOID param) {
	Info* pData = (Info*)param;
	COORD pos = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	WaitForSingleObject(pData->hTimer, INFINITE);
	int xtam[3];
	TCHAR tam[20];
	int i = 0, j = 0;
	do {
		if (pData->arrayGame == NULL) {
			_tprintf_s(TEXT("e nulo"));
		}

		WaitForSingleObject(pData->hMutexArray, INFINITE);
		/*
		GetConsoleScreenBufferInfo(pData->hStdout, &csbi);
		SetConsoleCursorPosition(pData->hStdout, pos);
		//SetConsoleTextAttribute(pData->hStdout, FOREGROUND_BLUE << pData->id);
		*/
		for (i = 0; i < COLUMNS - 1; i++) {
			TCHAR a = pData->arrayGame[pData->nFaixaResp][i];
			if(a == TEXT(' ')) {
				pData->arrayGame[pData->nFaixaResp][i] = _T(' ');
				if (i <= COLUMNS - 2)
					pData->arrayGame[pData->nFaixaResp][i + 1] = pData->o.c;
				else {
					pData->arrayGame[pData->nFaixaResp][0] = pData->o.c;
				}
			}
		}

		//pData->arrayGame[0][0] = TEXT(' ');
		/*
		SetConsoleTextAttribute(pData->hStdout, csbi.wAttributes);
		SetConsoleCursorPosition(pData->hStdout, csbi.dwCursorPosition);
		*/
		ReleaseMutex(pData->hMutexArray);
		Sleep(pData->veloc);
		break;
	} while (pData->end);
	WaitForSingleObject(pData->hMutexArray, INFINITE);
	ExitThread(1);
}
void lancaThread(FaixaVelocity dados, COORD posI, HANDLE hStdout) {
	objs o;
	o.c = TEXT('C');
	o.o = TEXT('O');
	o.s = TEXT('S');

	TCHAR** boardGameArray = (TCHAR**)malloc(sizeof(TCHAR*) * dados.faixa);
	for (int i = 0; i < dados.faixa; i++) {
		boardGameArray[i] = (TCHAR*)malloc(sizeof(TCHAR) * (COLUMNS+1)); 
		if (boardGameArray[i] == NULL) {
			printf("Memory allocation failed.\n");
			return 1;
		}
		DWORD random_num = rand() % (20 - 0 + 1) ;
		for (int j = 0; j < COLUMNS; j++) {
			if(j == random_num)
				boardGameArray[i][j] = o.c;
			else
				boardGameArray[i][j] = ' ';
		}
		boardGameArray[i][COLUMNS] = '\0';
	}

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

	posI.X = 0;
	posI.Y = 7;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hStdout, &csbi);
	SetConsoleCursorPosition(hStdout, posI);
	_tprintf_s(TEXT("-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
	for (int i = 0; i < dados.faixa; i++) {
		for (int j = 0; j < 20; j++) {
			_tprintf_s(TEXT(" %c "),boardGameArray[i][j]);
		}
		_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    -  \n"));

	}
	

	SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);

	Info* send = (Info*)malloc(dados.faixa * sizeof(Info));
	
	if (send == NULL) {
		printf_s(TEXT("Erro a criar condições para o jogo."));
		ExitProcess(1);
	}
	
	//Preencher array randomly 
	
	//
	for (int i = 0; i < dados.faixa; i++) {
		send[i].id = i;
		send[i].nFaixaResp = dados.faixa - i -1 ;
		send[i].arrayGame = boardGameArray;
		send[i].o = o;
		send[i].veloc = dados.faixa;
		//send[i].nFaixaResp = 0;
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
	do {
		posI.X = 0;
		posI.Y = 7;
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hStdout, &csbi);
		SetConsoleCursorPosition(hStdout, posI);
		_tprintf_s(TEXT("-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
		for (int i = 0; i < dados.faixa; i++) {
			for (int j = 0; j < 20; j++) {
				_tprintf_s(TEXT(" %c "), boardGameArray[i][j]);
			}
			_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    -  \n"));

		}
	} while (1);
	
	CloseHandle(hMutexArray);
	CloseHandle(hTimer);
	CloseHandle(hThreads);
	
	return;
}