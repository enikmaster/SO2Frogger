#include "servidor.h"
#include "..\Froggerino\froggerino.h"

DWORD WINAPI ThreadsFaixa(LPVOID param) {
	// TODO: Alterar o random
	srand(time(NULL));
	Info* pData = (Info*)param;
	COORD pos = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int i = 0, j = 0;
	WaitForSingleObject(pData->hEventStart, INFINITE);
	do {
		EnterCriticalSection(&pData->cs);
		if(pData->colocaObjeto) {
			do{
				// TODO: Alterar o random
				DWORD random_num = rand() % (20 - 0 + 1);
				if (pData->arrayGame[pData->nFaixaResp][random_num] == TEXT(' ')) {
					pData->arrayGame[pData->nFaixaResp][random_num] = pData->o.o;
					pData->colocaObjeto = FALSE;
					break;
				}
			} while (pData->colocaObjeto);
		}
		if(pData->moving){
			if (pData->sentido) {
				for (i = 0; i < COLUMNS; i++) {
					if (pData->arrayGame[pData->nFaixaResp][i] == pData->o.c) {
						if ((pData->arrayGame[pData->nFaixaResp][i+1] == pData->o.o))
							continue;
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
						if (pData->arrayGame[pData->nFaixaResp][i - 1] == pData->o.o)
							continue;
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
			SetEvent(pData->hEventSendToBoard);
			LeaveCriticalSection(&pData->cs);
		}
		else {
			LeaveCriticalSection(&pData->cs);
			if (!pData->moving && pData->allMoving) {
				WaitForSingleObject(pData->hEventPause, INFINITE);
				ResetEvent(pData->hEventPause);
			}
			else if(!pData->moving && !pData->allMoving){
				WaitForSingleObject(pData->hEventRemovePause, 3000);
				EnterCriticalSection(&pData->cs);
				pData->moving = TRUE;
				pData->allMoving = TRUE;
				ResetEvent(pData->hEventRemovePause);
				LeaveCriticalSection(&pData->cs);
			}
		}
		Sleep(700);
	} while (pData->end);

	ExitThread(1);
}

DWORD WINAPI LeComandosOperadoresThread(LPVOID param) {
	ControlData* pdata = (LPVOID*)param;
	BufferCell infoToMake;
	while (pdata->infoControl->end) {
		WaitForSingleObject(pdata->hReadSem, INFINITE);
		WaitForSingleObject(pdata->hMutex, INFINITE); //mutex unico da memória partilhada
		//Sera que preciso Mutex de protecao do buffer?
		CopyMemory(&infoToMake, &pdata->sharedMem->buffer[pdata->sharedMem->rP++], sizeof(pdata->sharedMem->buffer));
		if (pdata->sharedMem->rP == BUFFER_SIZE)
			pdata->sharedMem->rP = 0;
		ReleaseSemaphore(pdata->hWriteSem, 1, NULL);
		DWORD RANDOM;
		switch (infoToMake.f1)
		{
		case 1:
			pdata->infoControl[infoToMake.f2 - 1].moving = FALSE;
			break;
		case 2:
			pdata->infoControl[infoToMake.f2 - 1].sentido = (pdata->infoControl[infoToMake.f2 - 1].sentido ? 1 : 0) ? 0 : 1;
			break;
		case 3: 
			pdata->infoControl[infoToMake.f2 - 1].colocaObjeto = TRUE;
			break;
		case 4: 
			if (pdata->infoControl[infoToMake.f2 - 1].moving == FALSE) {
				pdata->infoControl[infoToMake.f2 - 1].moving = TRUE;
				SetEvent(pdata->infoControl[infoToMake.f2 - 1].hEventPause);
			}
			break;
		case 5:
			for (int i = 0; i < pdata->sharedMem->faixaMax; i++) {
				pdata->infoControl[i].moving = FALSE;
				pdata->infoControl[i].allMoving = FALSE;
			}
			break;
		default:
			break;
		}
		ReleaseMutex(pdata->hMutex);
	}
	return 0;
}

DWORD WINAPI ThreadVeTeclado(LPVOID param) {
	DWORD* x = (DWORD*)param;
	do {
		TCHAR hi[20];
		_fgetts(hi, 20, stdin);
		if (wcscmp(hi, TEXT("sair\n")) == 0)
			break;
	}while(1);
	*x = 1;
	ExitThread(0);
	
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
			ExitProcess(-1);
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
	if (hMutexArray == NULL) {
		_tprintf_s(TEXT("[ERRO] Erro a criar Mutex\n"));
		CloseHandle(hMutexArray);
		ExitProcess(-1);
	}
	HANDLE hEventStart = CreateEvent(NULL, TRUE, FALSE, TEXT("EventoThreads"));
	if (hEventStart == NULL) {
		_tprintf_s(TEXT("[ERRO] Erro a criar event\n"));
		CloseHandle(hMutexArray);
		CloseHandle(hEventStart);
		ExitProcess(-1);
	}
	HANDLE* hThreads = (HANDLE*)malloc(dados.faixa * sizeof(HANDLE));
	if (hThreads == NULL) {
		_tprintf_s(TEXT("[ERRO] Erro a alocar memória para jogo.\n"));
		CloseHandle(hMutexArray);
		CloseHandle(hEventStart);
		CloseHandle(hThreads);
		ExitProcess(-1);
	}
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
		_tprintf_s(TEXT("[ERRO] Erro a criar condições para o jogo."));
		CloseHandle(hMutexArray);
		CloseHandle(hEventStart);
		CloseHandle(hThreads);
		ExitProcess(1);
	}

	CRITICAL_SECTION cs_;
	InitializeCriticalSection(&cs_);

	HANDLE EnviaEvento = CreateEvent(NULL, TRUE, FALSE, NULL);

	for (int i = 0; i < dados.faixa; i++) {
		send[i].id = i;
		send[i].nFaixaResp = dados.faixa - i - 1;
		send[i].arrayGame = boardGameArray;
		send[i].o = o;
		send[i].veloc = (rand() % (4))+1;
		send[i].moving = TRUE;
		send[i].allMoving = TRUE;
		send[i].colocaObjeto = FALSE;
		send[i].sentido = 1;
		send[i].hMutexArray = hMutexArray;
		send[i].hEventStart = hEventStart;
		send[i].hEventPause = CreateEvent(NULL, TRUE, FALSE, NULL);
		send[i].hEventSendToBoard = EnviaEvento;
		send[i].cs = cs_; //rever com stor

		if (send[i].hEventPause == NULL) {
			_tprintf_s(TEXT("[ERRO] Erro a criar event\n"));
			CloseHandle(send[i].hEventPause);
			CloseHandle(hMutexArray);
			CloseHandle(hEventStart);
			CloseHandle(hThreads);
			ExitProcess(-1);
		}

		send[i].hEventRemovePause = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (send[i].hEventRemovePause == NULL) {
			_tprintf_s(TEXT("[ERRO] Erro a criar event\n"));
			CloseHandle(send[i].hEventRemovePause);
			CloseHandle(hMutexArray);
			CloseHandle(hEventStart);
			CloseHandle(hThreads);
			ExitProcess(-1);
		}
		send[i].hStdout = hStdout;
		send[i].end = 1;
		send[i].num = i;
		hThreads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadsFaixa, (LPVOID)&send[i], 0, &send[i].id);
		if (hThreads[i] == NULL) {
			_tprintf_s(TEXT("[ERRO] Não foi possível lançar as threads para o jogo iniciar.\n"));
			CloseHandle(send[i].hEventPause);
			CloseHandle(hMutexArray);
			CloseHandle(hEventStart);
			CloseHandle(hThreads);
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
	if (hLeitura == NULL) {
		_tprintf_s(TEXT("[ERRO] Erro ao criar thread de leitura.\n"));
		CloseHandle(hLeitura);
		ExitProcess(1);
	}
	TCHAR character;
	_tprintf_s(TEXT("Digite um enter para iniciar o jogo: "));
	wscanf_s(TEXT("%lc"), &character);
	SetEvent(hEventStart);

	//Ativar threads do jogo quando entrar um jogador ou dois
	DWORD x = 0;
	HANDLE fechar = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadVeTeclado, (LPVOID)&x, 0, NULL);
	do {
		
		WaitForSingleObject(EnviaEvento, INFINITE);
		WaitForSingleObject(hMutexArray, INFINITE);
		EnterCriticalSection(&cs_);
		for (int i = 0; i < dados.faixa; i++) {
			for (int j = 0; j < 20; j++) {
				//_tprintf_s(TEXT(" %c "), boardGameArray[i][j]);
				a.sharedMem->gameShared[i][j] = boardGameArray[i][j];
			}
		}
		LeaveCriticalSection(&cs_);
		ReleaseMutex(hMutexArray);
		SetEvent(a.hEvent);
		ResetEvent(EnviaEvento);
	} while (x==0);
	
	_tprintf_s(TEXT("Vai encerrar..."));

	for (int i = 0; i < dados.faixa; i++) {
		send[i].end = 0;
		WaitForSingleObject(hThreads[i], INFINITE);
	}
	CloseHandle(hMutexArray);
	CloseHandle(hEventStart);
	return;
}
