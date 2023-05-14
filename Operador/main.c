#include "operador.h"
#include "..\Froggerino\froggerino.h"

void showBG(TCHAR localBG[10][20], DWORD TamFaixa) {
	for (int i = 0; i < TamFaixa; i++) {
		TCHAR lvl0[100] = TEXT("");  
		for (int j = 0; j < 20; j++) {
			TCHAR temp[4];  
			temp[0] = TEXT(' ');
			temp[1] = localBG[i][j]; 
			temp[2] = TEXT(' '); 
			temp[3] = TEXT('\0');  

			wcscat_s(lvl0, 100, temp);  
		}
		_tprintf_s(TEXT("%s\n"), lvl0); 
		_tprintf_s(TEXT("-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
	} 
}

DWORD WINAPI GetInput(LPVOID param) {
	infoextra* pdata = (infoextra*)param;
	
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Error: CreateFileMapping (%d)\n"), GetLastError());
		return FALSE;
	}
	pdata->controlingData.hMapFile = hMapFile;
	pdata->controlingData.sharedMem = (SharedMem*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pdata->controlingData.sharedMem == NULL) {
		_tprintf(TEXT("Error: Alocação de memória deu erro\n"));
	}
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENTSHAREDMEM);
	if (hEvent == NULL) {
		ExitProcess(1);
	}

	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEXSHAREDMEM);
	pdata->controlingData.hEvent = hEvent;
	pdata->controlingData.hMutex = hMutex;
	TCHAR localBG[10][20];
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD pos;
	pos.X = 0;
	pos.Y = 5;	

	do {
		WaitForSingleObject(pdata->controlingData.hEvent, INFINITE);
		WaitForSingleObject(pdata->controlingData.hMutex, INFINITE);
		CopyMemory(&localBG, pdata->controlingData.sharedMem->gameShared, sizeof(pdata->controlingData.sharedMem->gameShared));
		ReleaseMutex(pdata->controlingData.hMutex);
		ResetEvent(pdata->controlingData.hEvent);
		WaitForSingleObject(pdata->hMutex, INFINITE);
		GetConsoleScreenBufferInfo(pdata->hStdout, &csbi);
		SetConsoleCursorPosition(pdata->hStdout, pos);
		_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
		showBG(localBG, pdata->controlingData.sharedMem->faixaMax);
		SetConsoleCursorPosition(pdata->hStdout, csbi.dwCursorPosition);
		ReleaseMutex(pdata->hMutex);
		Sleep(500);
	} while (1);
	
	CloseHandle(pdata->controlingData.hMapFile);
	ExitThread(1);
}

// tentativa n8...
Comando checkInput(TCHAR* msg, infoextra* info, DWORD* AltFaixa, DWORD maxFaixas) {
	TCHAR palavra[256] = { 0 };
	DWORD count = 0;
	TCHAR firstarg[256] = { 0 };
	TCHAR secondArgStr[6] = { 0 };
	DWORD secondArg = 0;
	int first_arg_index = 0;
	int second_arg_index = 0;

	for (int i = 0; msg[i] != TEXT('\n'); i++) {
		if (msg[i] == TEXT(' '))
			++count;
		else {
			if (count == 0) {
				if (first_arg_index < sizeof(firstarg) - 1) 
					firstarg[first_arg_index++] = msg[i];
				else
					return CMD_ERRO;
			}
			else if (count == 1) {
				if (second_arg_index < sizeof(secondArgStr) / sizeof(secondArgStr[0]) - 1)
					if (iswdigit(msg[i]))
						secondArgStr[second_arg_index++] = msg[i];
					else
						return CMD_ERRO;
				else 
					return CMD_ERRO;
			}
		}
		if (count > 2)
			return CMD_ERRO;
	}
	secondArgStr[second_arg_index] = TEXT('\0');
	secondArg = _tstoi(secondArgStr);
	firstarg[first_arg_index] = TEXT('\0');
	if (secondArg < 2 || secondArg > maxFaixas - 1 ) {
		return CMD_ERRO;
	}
	*AltFaixa = secondArg;
	_tcsupr_s(firstarg, sizeof(firstarg) / sizeof(firstarg[0]));
	if (wcscmp(firstarg, TEXT("PARAR")) == 0) {
		return CMD_PARAR;
	}
	else if (wcscmp(firstarg, TEXT("ADICIONAR")) == 0) {
		return CMD_ADICIONAR;
	}
	else if (wcscmp(firstarg, TEXT("INVERTER")) == 0) {
		return CMD_INVERTER;
	}
	else if (wcscmp(firstarg, TEXT("RETOMAR")) == 0) {
		return CMD_RETOMAR;
	}

	return CMD_ERRO;
}

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
	pos.X = 0;
	pos.Y = 0;
	BOOL succeed = FillConsoleOutputCharacter(hStdout, _T(' '), 80 * 26, pos, &res);
	if (!succeed) {
		_tprintf_s(TEXT("Error"));
		ExitProcess(-1);
	}
	_tprintf_s(TEXT("Operador"));

	
	HANDLE MutexShared = CreateMutex(NULL, FALSE, TEXT("MUTEXLOCAL"));

	
	infoextra extra;

	extra.hMutex = MutexShared;
	extra.hStdout = hStdout;
	HANDLE hThread = CreateThread(NULL, 0, GetInput, &extra, 0, NULL);
	extra.controlingData.hReadSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, SEM_READ_NAME);
	extra.controlingData.hWriteSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, SEM_WRITE_NAME);
	int count = 0;
	TCHAR msg[256];

	while (1) {
		BufferCell infoToServer;
		infoToServer.f1 = 0;
		infoToServer.f2 = 0;
		DWORD written;
		CONSOLE_SCREEN_BUFFER_INFO info;
		DWORD valid = 0;
		pos.X = 0;
		pos.Y = 1;
		SetConsoleCursorPosition(hStdout, pos);
		WaitForSingleObject(extra.hMutex, INFINITE);
		_tprintf_s(TEXT("Comando a enviar: "));
		ReleaseMutex(extra.hMutex);
		pos.X = 18;
		pos.Y = 1;
		SetConsoleCursorPosition(hStdout, pos);
		_fgetts(msg, sizeof(msg) / sizeof(TCHAR), stdin);
		WaitForSingleObject(extra.hMutex, INFINITE);
		if (count > 0) {
			pos.X = 0;
			pos.Y = 2;
			SetConsoleCursorPosition(hStdout, pos);
			FillConsoleOutputCharacter(hStdout, ' ', 50, pos, &written);
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
			default:
				_tprintf_s(TEXT("Comando invalido!"));
				valid = 0;
				break;
		}
		if(valid == 1){
			WaitForSingleObject(extra.controlingData.hWriteSem, INFINITE);
			WaitForSingleObject(extra.controlingData.hMutex, INFINITE);
			CopyMemory(&extra.controlingData.sharedMem->buffer[extra.controlingData.sharedMem->wP++], &infoToServer, sizeof(extra.controlingData.sharedMem->buffer));
			if (extra.controlingData.sharedMem->wP == BUFFER_SIZE)
				extra.controlingData.sharedMem->wP = 0;
			ReleaseMutex(extra.controlingData.hMutex);
			ReleaseSemaphore(extra.controlingData.hReadSem,1,NULL);
		}
		
		count++;
		pos.Y = 1;
		COORD start = { pos.X, pos.Y };
		GetConsoleScreenBufferInfo(hStdout, &info);
		DWORD numChars = info.dwSize.X - pos.X;
		FillConsoleOutputCharacter(hStdout, TEXT(' '), numChars, start, &written);
		ReleaseMutex(extra.hMutex);
	}
	CloseHandle(extra.hMutex);
	return 0;
}
