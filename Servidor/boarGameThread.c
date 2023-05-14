#include "servidor.h"
#include "..\Froggerino\froggerino.h"

DWORD WINAPI ThreadsFaixa(LPVOID param) {
	Info* pData = (Info*)param;
	COORD pos = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	
	int i = 0, j = 0;
	WaitForSingleObject(pData->hEventStart, INFINITE);
	do {
		WaitForSingleObject(pData->hMutexArray, INFINITE);
		if(pData->moving){
			if (pData->sentido) {
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
			}
			else {
				for (i = COLUMNS - 1; i >= 0; i--) {
					if (pData->arrayGame[pData->nFaixaResp][i] == pData->o.c) {
						pData->arrayGame[pData->nFaixaResp][i] = TEXT(' ');
						if (i == 0) {
							pData->arrayGame[pData->nFaixaResp][COLUMNS - 1] = pData->o.c;
						}
						else
							pData->arrayGame[pData->nFaixaResp][i - 1] = pData->o.c;
						break;
					}
				}
			}
		}
		else {
			ReleaseMutex(pData->hMutexArray);
			WaitForSingleObject(pData->hEventPause, INFINITE);
			ResetEvent(pData->hEventPause);
		}
		
		ReleaseMutex(pData->hMutexArray);
		Sleep(1000);
	} while (pData->end);
	//WaitForSingleObject(pData->hMutexArray, INFINITE);
	ExitThread(1);
}

DWORD WINAPI LeComandosOperadoresThread(LPVOID param) {
	ControlData* pdata = (LPVOID*)param;
	BufferCell infoToMake;
	while (1) {
		WaitForSingleObject(pdata->hReadSem, INFINITE);
		_tprintf_s(TEXT("Desbloqueou o hread\n"));
		WaitForSingleObject(pdata->hMutex, INFINITE);
		_tprintf_s(TEXT("Mutex desbloqueado\n"));
		//Sera que preciso Mutex de protecao do buffer?
		CopyMemory(&infoToMake, &pdata->sharedMem->buffer[pdata->sharedMem->rP++], sizeof(pdata->sharedMem->buffer));
		if (pdata->sharedMem->rP == BUFFER_SIZE)
			pdata->sharedMem->rP = 0;
		ReleaseSemaphore(pdata->hWriteSem, 1, NULL);
		_tprintf(TEXT("\n Leu isto:\nFuncao a relizar: %d\nFaixa a realizar: %d"), infoToMake.f1,infoToMake.f2);
		switch (infoToMake.f1)
		{
		case 1:
			pdata->infoControl[infoToMake.f2 - 1].moving = FALSE;
			break;
		case 2:
			pdata->infoControl[infoToMake.f2 - 1].sentido = (pdata->infoControl[infoToMake.f2 - 1].sentido ? 1 : 0) ? 0 : 1;
			break;
		case 3: 
			break;
		case 4: 
			if (pdata->infoControl[infoToMake.f2 - 1].moving == FALSE) {
				pdata->infoControl[infoToMake.f2 - 1].moving = TRUE;
				SetEvent(pdata->infoControl[infoToMake.f2 - 1].hEventPause);
			}
			break;
		default:
			break;
		}
		ReleaseMutex(pdata->hMutex);
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
		if (i == 0) {
			for (int j = 0; j < COLUMNS; j++) {
				if (j % 3 == 0)
					boardGameArray[i][j] = TEXT('W');
				else
					boardGameArray[i][j] = TEXT(' ');
			}
			boardGameArray[i][COLUMNS] = '\0';

			continue;
		}
		if (i == dados.faixa-1) {
			DWORD randm_n = 0;
			do {
				randm_n = rand() % (20 - 0 + 1);
			} while (randm_n == random_num);

			for (int j = 0; j < COLUMNS; j++) {
				if (j == random_num || j == randm_n)
					boardGameArray[i][j] = TEXT('S');
				else
					boardGameArray[i][j] = TEXT(' ');
			}
			boardGameArray[i][COLUMNS] = '\0';

			continue;
		}

		for (int j = 0; j < COLUMNS; j++) {
			if (j == random_num)
				boardGameArray[i][j] = o.c;
			else
				boardGameArray[i][j] = TEXT(' ');
		}
		boardGameArray[i][COLUMNS] = '\0';
	}

	if (boardGameArray == NULL) {
		_tprintf_s(TEXT("Erro a criar memória para o jogo\n"));
		ExitProcess(-1);
	}

	HANDLE hMutexArray = CreateMutex(NULL, FALSE, MUTEXSHAREDMEM);
	HANDLE hEventStart = CreateEvent(NULL, TRUE, FALSE, TEXT("EventoThreads"));

	HANDLE* hThreads = (HANDLE*)malloc(dados.faixa * sizeof(HANDLE));

	if (hMutexArray == NULL || hEventStart == NULL || hThreads == NULL) {
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
		send[i].moving = TRUE;
		send[i].sentido = 1;
		send[i].hMutexArray = hMutexArray;
		send[i].hEventStart = hEventStart;
		send[i].hEventPause = CreateEvent(NULL, TRUE, FALSE, NULL);
		send[i].hStdout = hStdout;
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
	a.infoControl = send;
	if (!initMemAndSync(&a, &send[0], dados.faixa)) {
		_tprintf_s(TEXT("Não foi possível criar memória partilhada.\n"));
		ExitProcess(-1);
	}
	a.hEvent = CreateEvent(NULL, TRUE, FALSE, EVENTSHAREDMEM);
	if (a.hEvent == NULL) {
		_tprintf_s(TEXT("Erro"));
		ExitProcess(1);
	}


	HANDLE hLeitura = CreateThread(NULL, 0, LeComandosOperadoresThread, (LPVOID)&a, 0, NULL); //proteger
	TCHAR character;
	_tprintf_s(TEXT("Digite um enter para iniciar o jogo: "));
	wscanf_s(TEXT("%lc"), &character);
	SetEvent(hEventStart);
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
	CloseHandle(hEventStart);
	CloseHandle(hThreads);

	return;
}