﻿#include "operador.h"


int _tmain(int argc, TCHAR** argv) {
#ifdef UNICODE
	DWORD x1, x2, x3;
	x1 = _setmode(_fileno(stdin), _O_WTEXT);
	x2 = _setmode(_fileno(stdout), _O_WTEXT);
	x3 = _setmode(_fileno(stderr), _O_WTEXT);
	if (x1 == -1 || x2 == -1 || x3 == -1)
		ExitProcess(-1);
#endif
	COORD pos;
	DWORD res;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if(hStdout == NULL) {
		_tprintf_s(TEXT("Error"));
		ExitProcess(-1);
	}
	pos.X = 0;
	pos.Y = 0;
	BOOL succeed = FillConsoleOutputCharacter(hStdout, _T(' '), WINDOWCOLUMNS * WINDOWROW, pos, &res);
	if (!succeed) {
		_tprintf_s(TEXT("[ERRO] Error"));
		ExitProcess(-1);
	}

	HANDLE MutexShared = CreateMutex(NULL, FALSE, TEXT("MUTEXLOCAL"));
	if (MutexShared == NULL) {
		_tprintf_s(TEXT("[ERRO] Erro a "));

		ExitProcess(-1);
	}
	
	infoextra extra;

	extra.hMutex = MutexShared;
	extra.hStdout = hStdout;
	extra.controlingData.fechar = FALSE;
	HANDLE hThread = CreateThread(NULL, 0, GetInput, &extra, 0, NULL);
	if (hThread == NULL) {
		_tprintf_s(TEXT("[ERRO] Erro a criar a thread.\n"));
		CloseHandle(hThread);
		ExitProcess(-1);
	}
	extra.controlingData.hReadSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, SEM_READ_NAME);
	if(extra.controlingData.hReadSem == NULL) {
		_tprintf_s(TEXT("[ERRO] Erro a abrir o Semaphore de leitura.\n"));
		CloseHandle(extra.controlingData.hReadSem);
		ExitProcess(-1);
	}
	extra.controlingData.hWriteSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, SEM_WRITE_NAME);
	if (extra.controlingData.hWriteSem == NULL) {
		_tprintf_s(TEXT("[ERRO] Erro a abrir o Semaphore de escrita.\n"));
		CloseHandle(extra.controlingData.hWriteSem);
		ExitProcess(-1);
	}
	int count = 0;
	TCHAR msg[TAMWORD];
	DWORD flagt = 0;
	while (1) {
		BufferCell infoToServer;
		infoToServer.f1 = 0;
		infoToServer.f2 = 0;
		DWORD written;
		CONSOLE_SCREEN_BUFFER_INFO info;
		DWORD valid = 0;
		pos.X = 0;
		pos.Y = YINPUTS;
		SetConsoleCursorPosition(hStdout, pos);
		WaitForSingleObject(extra.hMutex, INFINITE);
		_tprintf_s(TEXT("Comando a enviar: "));
		if (!ReleaseMutex(extra.hMutex)) {
			flagt = -1;
			break;
		}
		pos.X = YBOARD;
		pos.Y = YINPUTS;
		SetConsoleCursorPosition(hStdout, pos);
		_fgetts(msg, sizeof(msg) / sizeof(TCHAR), stdin);
		DWORD temp = WaitForSingleObject(extra.hMutex, 5000);
		if (temp == WAIT_TIMEOUT) {
			flagt = -1;
			break;
		}
		if (!ReleaseMutex(extra.hMutex)) {
			flagt = -1;
			break;
		}
		if (count > 0) {
			pos.X = 0;
			pos.Y = YINPUTS +1 ;
			SetConsoleCursorPosition(hStdout, pos);
			FillConsoleOutputCharacter(hStdout, ' ', TAMWORD, pos, &written);
		}
		Comando comando = checkInput(msg, &extra, &infoToServer.f2, extra.controlingData.sharedMem->faixaMax);
		switch (comando) {
			case CMD_PARAR:
				_tprintf_s(TEXT("Você pediu para parar a faixa nº%d com sucesso!"), infoToServer.f2);
				infoToServer.f1 = 1;
				valid = 1;
				break;
			case CMD_INVERTER:
				_tprintf_s(TEXT("Você pediu para inverter a faixa nº%d com sucesso!"), infoToServer.f2);
				infoToServer.f1 = 2;
				valid = 1;
				break;
			case CMD_ADICIONAR:
				_tprintf_s(TEXT("Você pediu para colocar objeto nº%d com sucesso!"), infoToServer.f2);
				infoToServer.f1 = 3;
				valid = 1;
				break;
			case CMD_RETOMAR:
				_tprintf_s(TEXT("Você pediu para retomar movimento da faixa nº%d com sucesso!"), infoToServer.f2);
				infoToServer.f1 = 4;
				valid = 1;
				break;
			case CMD_SAIR:
				_tprintf_s(TEXT("Vai encerrar!"));
				extra.controlingData.fechar = TRUE;
				break;
			case CMD_PAUSEALL:
				_tprintf_s(TEXT("Você pediu para pausar o movimento de todas as faixas com sucesso!"));
				infoToServer.f1 = 5;
				valid = 1;
				break;
			default:
				_tprintf_s(TEXT("[ERRO] Comando invalido!"));
				valid = 0;
				break;
		}
		if (extra.controlingData.fechar == TRUE) {
			ReleaseMutex(extra.hMutex);
			break;
		}
		if(valid == 1){
			WaitForSingleObject(extra.controlingData.hWriteSem, INFINITE);
			WaitForSingleObject(extra.controlingData.hMutex, INFINITE);
			CopyMemory(&extra.controlingData.sharedMem->buffer[extra.controlingData.sharedMem->wP++], &infoToServer, sizeof(extra.controlingData.sharedMem->buffer));
			if (extra.controlingData.sharedMem->wP == BUFFER_SIZE)
				extra.controlingData.sharedMem->wP = 0;
			ReleaseMutex(extra.controlingData.hMutex);
			ReleaseSemaphore(extra.controlingData.hReadSem, 1, NULL);
		}
		
		count++;
		pos.Y = YINPUTS;
		COORD start = { pos.X, pos.Y };
		GetConsoleScreenBufferInfo(hStdout, &info);
		DWORD numChars = info.dwSize.X - pos.X;
		FillConsoleOutputCharacter(hStdout, TEXT(' '), numChars, start, &written);
		ReleaseMutex(extra.hMutex);
	}
	if (flagt != -1) {
		WaitForSingleObject(hThread, 5000);
		CloseHandle(extra.hMutex);
	}
	CloseHandle(hThread);
	CloseHandle(hStdout);
	CloseHandle(extra.controlingData.hReadSem);
	CloseHandle(extra.controlingData.hWriteSem);

	return 0;
}
