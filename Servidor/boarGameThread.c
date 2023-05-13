#include "servidor.h"
#include "..\Froggerino\froggerino.h"

DWORD WINAPI ThreadsFaixa(LPVOID param) {
	Info* pData = (Info*)param;
	COORD pos = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	WaitForSingleObject(pData->hTimer, INFINITE);
	int i = 0, j = 0;
	do {
		//validacoes
		WaitForSingleObject(pData->hMutexArray, INFINITE);
		for (i = 0; i < COLUMNS; i++) {
			if (pData->arrayGame[pData->nFaixaResp][i] == pData->o.c) {
				pData->arrayGame[pData->nFaixaResp][i] = TEXT(' ');
				if (i == COLUMNS - 1) {
					pData->arrayGame[pData->nFaixaResp][0] = pData->o.c;
				}
				else
					pData->arrayGame[pData->nFaixaResp][i + 1] = pData->o.c;
				break;
			}
		}
		ReleaseMutex(pData->hMutexArray);
		//SetEvent para sinalizar main de que tem algo para atualizar
		Sleep(pData->veloc * 100);
	} while (pData->end);
	WaitForSingleObject(pData->hMutexArray, INFINITE);
	ExitThread(1);
}

DWORD WINAPI LeComandosOperadoresThread(LPVOID param) {
	ControlData* pdata = (LPVOID*)param;
	TCHAR leitura[256];
	while (1) {
		WaitForSingleObject(pdata->hReadSem, INFINITE);
		_tprintf_s(TEXT("Desbloqueou o hread\n"));
		WaitForSingleObject(pdata->hMutex, INFINITE);
		_tprintf_s(TEXT("Mutex desbloqueado\n"));
		//Sera que preciso Mutex de protecao do buffer?
		CopyMemory(&leitura, &pdata->sharedMem->buffer[pdata->sharedMem->rP++], sizeof(pdata->sharedMem->buffer));
		if (pdata->sharedMem->rP == BUFFER_SIZE)
			pdata->sharedMem->rP = 0;
		ReleaseMutex(pdata->hMutex);
		ReleaseSemaphore(pdata->hWriteSem, 1, NULL);
		_tprintf(TEXT("\n Leu isto: %s"), leitura);
	}
	return 0;
}

void lancaThread(FaixaVelocity dados, COORD posI, HANDLE hStdout) {
	objs o;
	o.c = TEXT('C');
	o.o = TEXT('O');
	o.s = TEXT('S');

	TCHAR** boardGameArray = (TCHAR**)malloc(sizeof(TCHAR*) * dados.faixa);
	for (int i = 0; i < dados.faixa; i++) {
		boardGameArray[i] = (TCHAR*)malloc(sizeof(TCHAR) * (COLUMNS + 1));
		if (boardGameArray[i] == NULL) {
			printf("Memory allocation failed.\n");
			return 1;
		}
		DWORD random_num = rand() % (20 - 0 + 1);
		for (int j = 0; j < COLUMNS; j++) {
			if (j == random_num)
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

	HANDLE hMutexArray = CreateMutex(NULL, FALSE, MUTEXSHAREDMEM);
	HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);

	HANDLE* hThreads = (HANDLE*)malloc(dados.faixa * sizeof(HANDLE));

	if (hMutexArray == NULL || hTimer == NULL || hThreads == NULL) {
		_tprintf_s(TEXT("Erro a criar condições para o jogo."));
		ExitProcess(1);
		//Criar funcao para encerrar tudo
	}

	posI.X = 0;
	posI.Y = 7;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hStdout, &csbi);
	SetConsoleCursorPosition(hStdout, posI);
	
	SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);

	Info* send = (Info*)malloc(dados.faixa * sizeof(Info));

	if (send == NULL) {
		printf_s(TEXT("Erro a criar condições para o jogo."));
		ExitProcess(1);
	}

	//Preencher array randomly 

	for (int i = 0; i < dados.faixa; i++) {
		send[i].id = i;
		send[i].nFaixaResp = dados.faixa - i - 1;
		send[i].arrayGame = boardGameArray;
		send[i].o = o;
		send[i].veloc = dados.faixa + i * 10;
		//send[i].nFaixaResp = 0;
		send[i].hMutexArray = hMutexArray;
		send[i].hTimer = hTimer;
		send[i].hStdout = hStdout;
		//send[i].direction = 1 -> direita //send[i].direction = 0;
		send[i].end = 1;
		hThreads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadsFaixa, (LPVOID)&send[i], 0, &send[i].id);
		if (hThreads[i] == NULL)
		{
			_tprintf_s(TEXT("Não foi possível lançar as threads para o jogo iniciar.\n"));
			ExitProcess(-1);
		}
	}
	/////Começar aqui a memória partilhada
	// verifica se o mutex foi criado com sucesso
	ControlData a;

	if (!initMemAndSync(&a, &send[0], dados.faixa)) {
		_tprintf_s(TEXT("Não foi possível criar memória partilhada.\n"));
		ExitProcess(-1);
	}
	a.hEvent = CreateEvent(NULL, TRUE, FALSE, EVENTSHAREDMEM);
	if (a.hEvent == NULL) {
		_tprintf_s(TEXT("Erro"));
		ExitProcess(1);
	}

	LARGE_INTEGER liArranca;
	liArranca.QuadPart = -5 * 10000000;
	HANDLE hLeitura = CreateThread(NULL, 0, LeComandosOperadoresThread, (LPVOID)&a, 0, NULL); //proteger
	SetWaitableTimer(hTimer, (LARGE_INTEGER*)&liArranca, 0, NULL, NULL, FALSE);
	//Ativar threads do jogo quando entrar um jogador ou dois
	do {
		WaitForSingleObject(hMutexArray, INFINITE);
		for (int i = 0; i < dados.faixa; i++) {
			for (int j = 0; j < 20; j++) {
				//_tprintf_s(TEXT(" %c "), boardGameArray[i][j]);
				a.sharedMem->gameShared[i][j] = boardGameArray[i][j];
			}

		}
		ReleaseMutex(hMutexArray, INFINITE);
		SetEvent(a.hEvent);
		//Sinalizar Evento de Operadores que podem copiar tabuleiro
	} while (1);

	CloseHandle(hMutexArray);
	CloseHandle(hTimer);
	CloseHandle(hThreads);

	return;
}