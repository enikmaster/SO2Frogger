#include "servidor.h"
#include "..\Froggerino\froggerino.h"
DWORD xG = 0;
DWORD WINAPI ThreadsFaixa(LPVOID param) {
	_tprintf_s(TEXT("[ID]: %d\n"), ++xG);
	// TODO: Alterar o random
	Info* pData = (Info*)param;
	COORD pos = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	BOOL start = TRUE;
	int i = 0, j = 0, z = 0;
	DWORD exit = 0;
	HANDLE hEventos[3];
	hEventos[2] = pData->a->hEventoStopAll;
	hEventos[1] = pData->a->hEventoTerminouJogo;
	hEventos[0] = pData->a->hEventoTerminouTempo;
	DWORD nCarros = 0;
	DWORD n = 0;
	BOOL reset = FALSE;
	pData->sentidoFaixa = TRUE;
	do {
		WaitForSingleObject(pData->a->hEventStart, INFINITE);
		z = 0;
		reset = FALSE;
		start = TRUE;
		
		DWORD random_num = rand() % (4+1);
		if (pData->nivel->lvlActual > 1) {
			if (random_num > 1)
				pData->sentidoFaixa = FALSE;
		}
		
		nCarros = pData->nivel->nCarros;
		EnterCriticalSection(&pData->a->x);
		do {
			if (z == 1)
			{
				DWORD X = WaitForMultipleObjects(3, hEventos, FALSE, pData->nivel->velocidade);
				EnterCriticalSection(&pData->a->x);
				if (X == WAIT_TIMEOUT) {
						start = TRUE;
				}
				else if (X >= WAIT_OBJECT_0) {
					X -= WAIT_OBJECT_0;
					switch (X)
					{
					case 0:
						reset = TRUE;
						break;
					default:
						reset = TRUE;
						exit = 1;
						break;
					}
				}
			}
			else
				z = 1;
			if (start) {
				n = 0;
				
				if (pData->colocaObjeto) {
					do {
						DWORD random_num = rand() % (20 - 0 + 1);
						if (pData->arrayGame[pData->nFaixaResp][random_num] == TEXT(' ')) {
							pData->arrayGame[pData->nFaixaResp][random_num] = pData->o.o;
							pData->colocaObjeto = FALSE;
						}
					} while (pData->colocaObjeto);
					SetEvent(pData->a->hEventoAtualiza);
				}
				if (pData->moving) {
					if (pData->sentidoFaixa) {
						for (i = 0; i < COLUMNS; i++) {
							if (pData->arrayGame[pData->nFaixaResp][i] == pData->o.c) {
								if ((pData->arrayGame[pData->nFaixaResp][i + 1] == pData->o.o || pData->arrayGame[pData->nFaixaResp][i + 1] == pData->o.c))
								{
									continue;
								}
								
								if (i == COLUMNS - 1) {
									if (pData->arrayGame[pData->nFaixaResp][0] == TEXT("S")) {
										if (!pData->sapos[1].activo) {
											pData->sapos[0].vidas -= 1;
											pData->sapos[0].pos_atual.X = pData->sapos[0].pos_inicial.X;
											pData->sapos[0].pos_atual.Y = pData->sapos[0].pos_inicial.Y;
										}
										else {
											for (int k = 0; k < 2; k++) {
												if (pData->sapos[k].pos_atual.X == pData->nFaixaResp && pData->sapos[k].pos_atual.Y == 0)
												{
													pData->sapos[k].vidas -= 1;
													pData->sapos[k].pos_atual.X = pData->sapos[k].pos_inicial.X;
													pData->sapos[k].pos_atual.Y = pData->sapos[k].pos_inicial.Y;
												}

											}
										}
									}

									if (pData->arrayGame[pData->nFaixaResp][0] == pData->o.c || pData->arrayGame[pData->nFaixaResp][0] == pData->o.o) { 
										break;
									}
									pData->arrayGame[pData->nFaixaResp][i] = TEXT(' ');
									pData->arrayGame[pData->nFaixaResp][0] = pData->o.c;
									break;
								}
								else
								{
									if (pData->arrayGame[pData->nFaixaResp][i + 1] == pData->o.s) {
										if (!pData->sapos[1].activo) {
											pData->sapos[0].vidas -= 1;
											pData->sapos[0].pos_atual.X = pData->sapos[0].pos_inicial.X;
											pData->sapos[0].pos_atual.Y = pData->sapos[0].pos_inicial.Y;
										}
										else {
											for (int k = 0; k < 2; k++) {
												if (pData->sapos[k].pos_atual.X == pData->nFaixaResp && pData->sapos[k].pos_atual.Y == 0)
												{
													pData->sapos[k].vidas -= 1;
													pData->sapos[k].pos_atual.X = pData->sapos[k].pos_inicial.X;
													pData->sapos[k].pos_atual.Y = pData->sapos[k].pos_inicial.Y;
												}

											}
										}
									}
									pData->arrayGame[pData->nFaixaResp][i] = TEXT(' ');
									pData->arrayGame[pData->nFaixaResp][++i] = pData->o.c;
								}
								
							}
						}

					}
					else {
						for (i = COLUMNS - 1; i >= 0; i--) {
							if(pData->arrayGame[pData->nFaixaResp][i] == pData->o.c) {
								if ((pData->arrayGame[pData->nFaixaResp][i -1] == pData->o.o || pData->arrayGame[pData->nFaixaResp][i -1] == pData->o.c))
								{
									continue;
								}
								if (i == 0) {
									if (pData->arrayGame[pData->nFaixaResp][COLUMNS - 1] == pData->o.s) {
										if (!pData->sapos[1].activo) {
											pData->sapos[0].vidas -= 1;
											pData->sapos[0].pos_atual.X = pData->sapos[0].pos_inicial.X;
											pData->sapos[0].pos_atual.Y = pData->sapos[0].pos_inicial.Y;
										}
										else {
											for (int k = 0; k < 2; k++) {
												if (pData->sapos[k].pos_atual.X == pData->nFaixaResp && pData->sapos[k].pos_atual.Y == COLUMNS-1)
												{
													pData->sapos[k].vidas -= 1;
													pData->sapos[k].pos_atual.X = pData->sapos[k].pos_inicial.X;
													pData->sapos[k].pos_atual.Y = pData->sapos[k].pos_inicial.Y;
												}

											}
										}
									}
									if (pData->arrayGame[pData->nFaixaResp][COLUMNS-1] == pData->o.c || pData->arrayGame[pData->nFaixaResp][COLUMNS - 1] == pData->o.o) {
										break;
									}
									pData->arrayGame[pData->nFaixaResp][i] = TEXT(' ');
									pData->arrayGame[pData->nFaixaResp][COLUMNS-1] = pData->o.c;
									break;
								}
								else
								{
									if (pData->arrayGame[pData->nFaixaResp][i-1] == pData->o.s) {
										if (!pData->sapos[1].activo) {
											pData->sapos[0].vidas -= 1;
											pData->sapos[0].pos_atual.X = pData->sapos[0].pos_inicial.X;
											pData->sapos[0].pos_atual.Y = pData->sapos[0].pos_inicial.Y;
										}
										else {
											for (int k = 0; k < 2; k++) {
												if (pData->sapos[k].pos_atual.X == pData->nFaixaResp && pData->sapos[k].pos_atual.Y == i)
												{
													pData->sapos[k].vidas -= 1;
													pData->sapos[k].pos_atual.X = pData->sapos[k].pos_inicial.X;
													pData->sapos[k].pos_atual.Y = pData->sapos[k].pos_inicial.Y;
												}

											}
										}
									}
									pData->arrayGame[pData->nFaixaResp][i] = TEXT(' ');
									pData->arrayGame[pData->nFaixaResp][--i] = pData->o.c;
									continue;
								}

							}
						}
					}
					LeaveCriticalSection(&pData->a->x);
					SetEvent(pData->a->hEventoAtualiza);
					SetEvent(pData->a->hEventoEscreveSapos);
					
				}
				else {
					LeaveCriticalSection(&pData->a->x);
					if (!pData->moving && pData->allMoving) {
						WaitForSingleObject(pData->hEventPause, INFINITE);
						ResetEvent(pData->hEventPause);
					}
					else{
						Sleep(3000);
						pData->moving = TRUE;
						pData->allMoving = TRUE;
					}
				}

			}
		} while (!reset);
	} while (exit == 0);
	_tprintf_s(TEXT("Thread a dar delete"));
	ExitThread(1);
}
DWORD WINAPI EstadoTabuleiro(LPVOID param) { //inicializar esta thread para fazer gestão do tabuleiro sempre que houver um nivel novo

	Info* pdata = (Info*)param;
	HANDLE hEventos[3];
	hEventos[0] = pdata->a->hEventoTerminouTempo;
	hEventos[1] = pdata->a->hEventoTerminouJogo;
	hEventos[2] = pdata->a->hEventoStopAll;
	DWORD numeroRandom;
	DWORD flag = 0;
	DWORD noRepeat[6] = { -1,-1,-1,-1,-1,- 1 };
	for (int i = pdata->numFaixasTotal ; i > 0; i--) {
		for (int j = 0; j < pdata->nivel->nCarros; j++) {
			do {
				 numeroRandom = rand() % (20);
				if (pdata->arrayGame[i][numeroRandom] == TEXT(' '));
				{
					pdata->arrayGame[i][numeroRandom] = pdata->o.c;
					break;
				}
			} while (1);
		}
	}
	srand(time(NULL));
	while (1) {
		DWORD X = WaitForMultipleObjects(3, hEventos, FALSE, INFINITE);
		if (X >= WAIT_OBJECT_0)
			X -= WAIT_OBJECT_0;
		if (X == 1 || X == 2) {
			ExitThread(0);
		}
		EnterCriticalSection(&pdata->a->x);
		DWORD last_number = -1; 
		for (int i = pdata->numFaixasTotal ; i > 0; i--) {
			for (int j = 0; j < COLUMNS; j++) {
				pdata->arrayGame[i][j] = TEXT(' ');
			}
		}
		
		for (int i = pdata->numFaixasTotal; i > 0; i--) {
			for (int j = 0; j < pdata->nivel->nCarros; j++) {
				do {
					numeroRandom = rand() % (18 - 2 + 1) + 2;
					if (numeroRandom % 2 != 0 )
						 numeroRandom += 1;
					flag = 0;
					for (int k = 0; k < pdata->nivel->nCarros; k++)
						if (numeroRandom == noRepeat[k])
							flag = -1;
					if (flag == -1)
						 continue;
					if((pdata->arrayGame[i][numeroRandom + 1] == TEXT(' ')))
					{
						if ((pdata->arrayGame[i][numeroRandom] == TEXT(' ')));
						{
							pdata->arrayGame[i][numeroRandom] = TEXT('C');
							noRepeat[j] = numeroRandom;
							break;
						}
						flag = -1;
					}
						
				} while (flag = -1);
			}
			for (int l = 0; l < 6; l++)
				noRepeat[l] = -1;
		}
	
		ResetEvent(hEventos[X]);
		SetEvent(pdata->a->hEventStart);
		SetEvent(pdata->a->hEventoAtualiza);
		SetEvent(pdata->a->hEventoEscreveSapos);
		LeaveCriticalSection(&pdata->a->x);
	}

}
DWORD WINAPI LeComandosOperadoresThread(LPVOID param) {
	ControlData* pdata = (LPVOID*)param;
	BufferCell infoToMake;
	while (pdata->infoControl->end) {
		WaitForSingleObject(pdata->hReadSem, INFINITE);
		WaitForSingleObject(pdata->hMutex, INFINITE); //mutex unico da memória partilhada
		CopyMemory(&infoToMake, &pdata->sharedMem->buffer[pdata->sharedMem->rP++], sizeof(pdata->sharedMem->buffer));
		if (pdata->sharedMem->rP == BUFFER_SIZE)
			pdata->sharedMem->rP = 0;
		ReleaseSemaphore(pdata->hWriteSem, 1, NULL);
		DWORD RANDOM;
		EnterCriticalSection(&pdata->infoControl->a->x);
		switch (infoToMake.f1)
		{
		case 1:
			pdata->infoControl[infoToMake.f2 - 1].moving = FALSE;
			break;
		case 2:
			pdata->infoControl[infoToMake.f2 - 1].sentidoFaixa = (pdata->infoControl[infoToMake.f2 - 1].sentidoFaixa ? 1 : 0) ? 0 : 1;
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
			for (int i = 0; i <= pdata->sharedMem->faixaMax; i++) {
				pdata->infoControl[i].moving = FALSE;
				pdata->infoControl[i].allMoving = FALSE;
			}
			break;
		default:
			break;
		}
		ReleaseMutex(pdata->hMutex);
		LeaveCriticalSection(&pdata->infoControl->a->x);
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
	} while (1);
	*x = 1;
	ExitThread(0);
}
DWORD WINAPI ControlaPipesF(LPVOID param) {
	ControlaPipes* pdata = (ControlaPipes*)param;
	DWORD dwWait, cbRet;
	BOOL Sucesso;
	OVERLAPPED ov;
	DWORD x = 0, n, y;
	HANDLE hEventoss[INSTANCES+2];
	HANDLE hEvento = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NULL);

	if (hEvento == NULL) {
		_tprintf_s(TEXT("[ERRO] Erro ao criar evento.\n"));
		ExitThread(-1);
	}

	for (DWORD i = 0; i < INSTANCES; i++) {
		pdata->pipeMgm[i].hEvent = CreateEvent(
			NULL,
			TRUE,
			FALSE,
			NULL);
		if (pdata->pipeMgm[i].hEvent == NULL) {
			_tprintf_s(TEXT("[ERRO] Erro ao criar evento para Namedpipe Instancia falhou.\n"));
			ExitThread(-1);
		}
		pdata->pipeMgm[i].hEventoThread = CreateEvent(
			NULL,
			TRUE,
			FALSE,
			NULL);

		if (pdata->pipeMgm[i].hEventoThread == NULL) {
			_tprintf_s(TEXT("[ERRO] Erro ao criar evento para Namedpipe Instancia falhou.\n"));
			ExitThread(-1);
		}


		pdata->pipeMgm[i].hPipeInst = CreateNamedPipe(
			NOMEPIPE,
			PIPE_ACCESS_DUPLEX |     // read/write access 
			FILE_FLAG_OVERLAPPED,    // overlapped mode 
			PIPE_TYPE_MESSAGE |      // message-type pipe 
			PIPE_READMODE_MESSAGE |  // message-read mode 
			PIPE_WAIT,               // blocking mode 
			INSTANCES,               // number of instances 
			BUFSIZE * sizeof(TCHAR), // output buffer size 
			BUFSIZE * sizeof(TCHAR), // input buffer size 
			PIPE_TIMEOUT,            // client time-out 
			NULL);                   // default security attributes 

		if (pdata->pipeMgm[i].hPipeInst == INVALID_HANDLE_VALUE) {
			_tprintf_s(TEXT("[ERRO] Erro ao criar instancia de namedpipe.\n"));
			ExitThread(-1);
		}

		pdata->pipeMgm[i].ligado = FALSE;
		ZeroMemory(&pdata->pipeMgm[i].oOverlap, sizeof(OVERLAPPED));
		pdata->pipeMgm[i].oOverlap.hEvent = pdata->pipeMgm[i].hEvent;

		if (!ConnectNamedPipe(pdata->pipeMgm[i].hPipeInst, &pdata->pipeMgm[i].oOverlap)) {
			if (GetLastError() != ERROR_IO_PENDING) {
				_tprintf_s(TEXT("Falhou a conexão ao pipe.\n"));
				ExitProcess(-1);
			}
		}

	}
	HANDLE eventoFecha = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NULL);
	HANDLE hEventosMul[2];
	hEventosMul[0] = pdata->pipeMgm[0].hEvent;
	hEventosMul[1] = pdata->pipeMgm[1].hEvent;
	hEventoss[3] = eventoFecha;
	hEventoss[2] = pdata->gere->hEventoEscreveSapos;
	DWORD flagg = 1;
	while (1) {
		for (y = 0, n = 0; y < INSTANCES; y++)
			if (pdata->pipeMgm[y].hPipeInst != NULL)
				hEventoss[n++] = pdata->pipeMgm[y].hEvent;
		DWORD i = WaitForMultipleObjects(4, hEventoss, FALSE, INFINITE);
		if (i >= WAIT_OBJECT_0) {
			i -= WAIT_OBJECT_0;
			// a testar
			if (i == 3) {
				break;
			}
			if (i == 2) {
				flagg = 0;
				i = 0;
			}
			GetOverlappedResult(pdata->pipeMgm[i].hPipeInst, &pdata->pipeMgm[i].oOverlap, &pdata->pipeMgm[i].cbRead, FALSE);
			if (!pdata->pipeMgm[i].ligado ) {
				if (i == 0) {
					pdata->sapoAControlar = 0;
				}
				else
					pdata->sapoAControlar = 1;
				ResumeThread(pdata->ThreadsParaSapo[i]);
				pdata->pipeMgm[i].ligado = TRUE;
				ZeroMemory(&pdata->pipeMgm[i].oOverlap, sizeof(OVERLAPPED));
				pdata->pipeMgm[i].oOverlap.hEvent = pdata->pipeMgm[i].hEvent;
				ReadFile(pdata->pipeMgm[i].hPipeInst, pdata->pipeMgm[i].chRequest, BUFSIZE, &pdata->pipeMgm[i].cbRead, &pdata->pipeMgm[i].oOverlap);

			}
			else {
				if (pdata->pipeMgm[i].cbRead > 0 && i!=2 && flagg == 1) {
					CharUpperBuff(pdata->pipeMgm[i].chRequest, (DWORD)_tcslen(pdata->pipeMgm[i].chRequest));

					for (x = 0; x < INSTANCES; x++)
						if (pdata->pipeMgm[x].hPipeInst != NULL && pdata->pipeMgm[x].ligado) {
							ZeroMemory(&ov, sizeof(OVERLAPPED));
							ov.hEvent = hEvento;
							ResetEvent(hEvento);
				
							Sucesso = WriteFile(pdata->pipeMgm[x].hPipeInst, pdata->pipeMgm[i].chRequest, (DWORD)(_tcslen(pdata->pipeMgm[i].chRequest) + 1) * sizeof(TCHAR), &pdata->pipeMgm[i].cbToWrite, &ov);
							if (!Sucesso && GetLastError() == ERROR_IO_PENDING) {
								WaitForSingleObject(hEvento, INFINITE);
								GetOverlappedResult(pdata->pipeMgm[x].hPipeInst, &ov, &pdata->pipeMgm[i].cbRead, FALSE);
							}
						}
					ZeroMemory(&pdata->pipeMgm[i].oOverlap, sizeof(OVERLAPPED));
					pdata->pipeMgm[i].oOverlap.hEvent = pdata->pipeMgm[i].hEvent;
					ReadFile(pdata->pipeMgm[i].hPipeInst, pdata->pipeMgm[i].chRequest, BUFSIZE, &pdata->pipeMgm[i].cbRead, &pdata->pipeMgm[i].oOverlap);
				}
				else if (flagg == 0) {
					EnterCriticalSection(&pdata->gere->x);
					DWORD index = 0;
					for (int y = 0; y < pdata->nFaixas +2; y++) {
						for (int j = 0; j < COLUMNS; j++) {
							pdata->chReply[index++] = pdata->GameBoard[y][j];
						}
					}
					pdata->chReply[index] = '\0';
					LeaveCriticalSection(&pdata->gere->x);
					index = 0;
					for (int i = 0; i < INSTANCES; i++) {
						if (pdata->pipeMgm[i].hPipeInst != NULL && pdata->pipeMgm[i].ligado) {
							ZeroMemory(&ov, sizeof(OVERLAPPED));
							ov.hEvent = hEvento;
							ResetEvent(hEvento);
							Sucesso = WriteFile(pdata->pipeMgm[i].hPipeInst, pdata->chReply, (DWORD)(_tcslen(pdata->chReply) + 1) * sizeof(TCHAR), &pdata->cbToWrite, &ov);
							if (!Sucesso && GetLastError() == ERROR_IO_PENDING) {
								WaitForSingleObject(hEvento, INFINITE);
								GetOverlappedResult(pdata->pipeMgm[i].hPipeInst, &ov, &pdata->pipeMgm[i].cbRead, FALSE);
							}
							ZeroMemory(&pdata->pipeMgm[i].oOverlap, sizeof(OVERLAPPED));
							pdata->pipeMgm[i].oOverlap.hEvent = pdata->pipeMgm[i].hEvent;
							ReadFile(pdata->pipeMgm[i].hPipeInst, pdata->pipeMgm[i].chRequest, BUFSIZE, &pdata->pipeMgm[i].cbRead, &pdata->pipeMgm[i].oOverlap);
						}
					}
					flagg = 1;
					ResetEvent(pdata->gere->hEventoEscreveSapos);
				}
				else {

					DisconnectNamedPipe(pdata->pipeMgm[i].hPipeInst);
					pdata->pipeMgm[i].ligado = FALSE;
					ZeroMemory(&pdata->pipeMgm[i].oOverlap, sizeof(OVERLAPPED));
					pdata->pipeMgm[i].oOverlap.hEvent = pdata->pipeMgm[i].hEvent;
					ConnectNamedPipe(pdata->pipeMgm[i].hPipeInst, &pdata->pipeMgm[i].oOverlap);
				}
			}
		}
	}
	WaitForMultipleObjects(INSTANCES, pdata->ThreadsParaSapo, TRUE, INFINITE);
	ExitThread(0);
}
DWORD WINAPI ThreadsParaSapo(LPVOID param) {
	ControlaPipes* pdata = (LPVOID*)param;
	SAPO* sapoAcontrolar;
	DWORD z = 0;
	if (pdata->sapoAControlar == 0) {
		sapoAcontrolar = &pdata->saposa;
		z = 0;
	}
	else
	{
		sapoAcontrolar = &pdata->saposb;
		z = 1;
	}
	BOOL sucesso;
	DWORD ret;
	TCHAR buf[256];
	DWORD nBytes;
	SetEvent(pdata->gere->hEventStart);
	while (1) {
		WaitForSingleObject(pdata->pipeMgm[z].hEventoThread, INFINITE);
		SetEvent(pdata->pipeMgm[z].hEvent);
	}

	// Clean up and close the pipe handle
	DisconnectNamedPipe(pdata->pipeMgm[z].hPipeInst);
	CloseHandle(pdata->pipeMgm[z].hPipeInst);
	CloseHandle(pdata->pipeMgm[z].oOverlap.hEvent);
	ExitThread(0);
}
DWORD WINAPI ThreadTempo(LPVOID param) {
	Info* pdata = (Info*)param;
	DWORD exit = 0;
	DWORD z = 0;
	HANDLE hEventos[2];
	hEventos[0] = pdata->a->hEventoStopAll;
	hEventos[1] = pdata->a->hEventoTerminouJogo; //pois os sapos quando chegarem ambos à casa de chegada vão ter oportunidade de passar logo ao proximo nivel
	do {
		if(z==0)
			WaitForSingleObject(pdata->a->hEventStart, INFINITE);
		z = 1;
		do {
			if (z == 1 || z == 2) {
				DWORD x = WaitForMultipleObjects(2, hEventos, FALSE, 1000);
				if (x != WAIT_TIMEOUT) {
					pdata->end = 1;
					ExitThread(0);
				}
			}
			EnterCriticalSection(&pdata->a->x);
			pdata->nivel->tempo -= 1000;
			if (pdata->nivel->tempo == 0 && pdata->nivel->lvlActual == NIVEISDEJOGO) {
				SetEvent(pdata->a->hEventoTerminouJogo);
				pdata->end = 0;
				exit = 1;
				LeaveCriticalSection(&pdata->a->x);
				break;
			}
			else if (pdata->nivel->tempo == 0) {
				++(pdata->nivel);
				pdata->sapos[0].temp = pdata->nivel->tempo;
				if (pdata->sapos[1].activo)
					pdata->sapos[1].temp = pdata->nivel->tempo;
				z = 2; 
				LeaveCriticalSection(&pdata->a->x);
				SetEvent(pdata->a->hEventoTerminouTempo);
				break;
			}
			LeaveCriticalSection(&pdata->a->x);
			_tprintf_s(TEXT("Nivel: %d  & Tempo: %d\n"), pdata->nivel->lvlActual, pdata->nivel->tempo / 1000);
		} while (z == 1);
		//ResetEvent(pdata->a->hEventoTerminouTempo);
	} while (exit == 0);
	ExitProcess(0);
}

void lancaThread(FaixaVelocity dados, COORD posI, HANDLE hStdout) {
	objs o; 
	o.c = TEXT('C'); 
	o.o = TEXT('O'); 
	o.s = TEXT('S');

	TCHAR** boardGameArray = (TCHAR**)malloc(sizeof(TCHAR*) * dados.faixa);

	for (int i = 0; i <= (dados.faixa + 2); i++) {
		boardGameArray[i] = (TCHAR*)malloc(sizeof(TCHAR) * (COLUMNS + 1));
		if (boardGameArray[i] == NULL) {
			printf("Memory allocation failed.\n");
			ExitProcess(-1);
		}
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
		if (i == dados.faixa + 1 ) {
			for (int j = 0; j < COLUMNS; j++) {
				if (j % 3 == 0)
					boardGameArray[i][j] = TEXT('S');
				else
					boardGameArray[i][j] = TEXT(' ');
			}
			boardGameArray[i][COLUMNS] = '\0';
			break;
		}
		else
			for (int j = 0; j < COLUMNS; j++)
				boardGameArray[i][j] = TEXT(' ');
		boardGameArray[i][COLUMNS] = '\0';
	}
	
	//#############################################

	if (boardGameArray == NULL) {
		_tprintf_s(TEXT("Erro a criar memória para o jogo\n"));
		ExitProcess(-1);
	}
	//#############################################

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
	//#############################################

	CRITICAL_SECTION cs_;
	InitializeCriticalSection(&cs_);
	//#############################################
	Eventos_Mutexs allEventos;
	allEventos.hEventoAtualiza = CreateEvent(NULL, TRUE, FALSE, NULL);
	allEventos.hMutexArrayJogo = hMutexArray;
	allEventos.hEventStart = hEventStart;
	allEventos.hEventoEscreveSapos = CreateEvent(NULL, TRUE, FALSE, NULL);
	allEventos.hStdout = hStdout;
	allEventos.hEventoTerminouJogo = CreateEvent(NULL, TRUE, FALSE, TERMINOUJOGO);
	allEventos.hEventoTerminouTempo = CreateEvent(NULL, TRUE, FALSE, TERINOUTEMPO);
	allEventos.x = cs_;
	allEventos.hEventoStopAll = CreateEvent(NULL, TRUE, FALSE, TEXT("STOPALL"));
	if (allEventos.hEventoTerminouJogo == NULL || allEventos.hEventoTerminouJogo == NULL || allEventos.hEventoAtualiza == NULL) {
		_tprintf_s(TEXT("[ERRO] Não reune as condiçoes para iniciar o servidor\n"));
		ExitProcess(-1);
	}

	//#############################################
	pos aa; 
	pos bb;
	SAPO sapos[2];
	Nivel niveis[5];
	PreencheSapos(sapos, dados.faixa);
	PreencheNiveis(niveis, sapos);
	//#############################################

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
	ControlaPipes cp;
	cp.saposa = &sapos[0];
	cp.saposb = &sapos[1];
	cp.nFaixas = dados.faixa;
	DWORD flagEndAll = 0;

	for (int i = 0; i < dados.faixa ; i++) {
		send[i].id = i;
		send[i].nFaixaResp = dados.faixa - i;
		send[i].arrayGame = boardGameArray;
		send[i].o = o;
		send[i].end = &flagEndAll;
		send[i].moving = TRUE;
		DWORD random_num = rand() % (2);
		if (random_num == 1)
			send[i].sentidoFaixa = TRUE;
		else
			send[i].sentidoFaixa = FALSE;
		send[i].numFaixasTotal = dados.faixa;
		send[i].allMoving = TRUE;
		send[i].colocaObjeto = FALSE;
		send[i].nivel = niveis;
		send[i].hEventPause = CreateEvent(NULL, TRUE, FALSE, NULL);
		send[i].hEventPauseGlobal = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (send[i].hEventPause == NULL || send[i].hEventPauseGlobal == NULL) {
			_tprintf_s(TEXT("[ERRO] Não foi possível criar os eventos para controlo de faixas\n"));
			ExitProcess(-1);
		}
		send[i].sapos = sapos;
		send[i].a = &allEventos;
		hThreads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadsFaixa, (LPVOID)&send[i], 0, &send[i].id);
		if (hThreads[i] == NULL) {
			_tprintf_s(TEXT("[ERRO] Não foi possível lançar as threads para o jogo iniciar.\n"));
			CloseHandle(hMutexArray);
			CloseHandle(hEventStart);
			CloseHandle(hThreads);
			ExitProcess(-1);
		}
	}
	HANDLE hThreadGereTempo = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadTempo, (LPVOID)&send[0], 0, &send[0].id);
	if (hThreadGereTempo == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possível criar a gestão de tempo!\n"));
		ExitProcess(-1);
	}

	HANDLE hThreadEstadoJogo = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EstadoTabuleiro, (LPVOID)&send[0], 0, &send[0].id);

	if (hThreadEstadoJogo == NULL) {
		_tprintf_s(TEXT("[ERRO] Criando Thread para gestão do tabuleiro\n"));
		ExitProcess(-1);
	}

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

	HANDLE hLeitura = CreateThread(NULL, 0, LeComandosOperadoresThread, (LPVOID)&a, 0, &send[0].id);
	if (hLeitura == NULL) {
		_tprintf_s(TEXT("[ERRO] Erro ao criar thread de leitura.\n"));
		CloseHandle(hLeitura);
		ExitProcess(-1);
	}
	TCHAR character;
	// 
	PIPEINST Pinstacias[INSTANCES];
	//

	cp.GameBoard = boardGameArray;
	cp.gere = &allEventos;
	cp.pipeMgm = &Pinstacias;

	for (int i = 0; i < INSTANCES; i++) {
		cp.ThreadsParaSapo[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadsParaSapo, (LPVOID)&cp, CREATE_SUSPENDED, 0);
		if (cp.ThreadsParaSapo[i] == NULL) {
			_tprintf_s(TEXT("[ERRO] Erro ao criar threads para gerir os sapos. \n"));
			CloseHandle(cp.ThreadsParaSapo[i]);
			ExitProcess(-1);
		}
	}

	HANDLE ThreadPipes = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ControlaPipesF, (LPVOID)&cp, 0, &cp.pipeMgm->cbToWrite);
	if (ThreadPipes == NULL) {
		_tprintf_s(TEXT("[ERRO] Erro ao criar thread NamedPipes. \n"));
		CloseHandle(ThreadPipes);
		ExitProcess(-1);
	}
	DWORD x = 0;
	HANDLE fechar = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadVeTeclado, (LPVOID)&x, 0, NULL);
	do {

		WaitForSingleObject(allEventos.hEventoAtualiza, INFINITE);
		WaitForSingleObject(hMutexArray, INFINITE);
		EnterCriticalSection(&cs_);
		for (int i = 0; i < dados.faixa + 2; i++) {
			for (int j = 0; j < 20; j++) {
				a.sharedMem->gameShared[i][j] = boardGameArray[i][j];
			}
			
		}

		LeaveCriticalSection(&cs_);
		ReleaseMutex(hMutexArray);
		SetEvent(a.hEvent);
		ResetEvent(allEventos.hEventoAtualiza);

	} while (x == 0);

	_tprintf_s(TEXT("Vai encerrar..."));

	for (int i = 0; i < dados.faixa; i++) {
		send[i].end = 0;
		WaitForSingleObject(hThreads[i], INFINITE);
	}
	CloseHandle(hMutexArray);
	CloseHandle(hEventStart);
	return;
}