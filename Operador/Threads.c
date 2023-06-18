#include "operador.h"

DWORD WINAPI GetInput(LPVOID param) {
	infoextra* pdata = (infoextra*)param;

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("[ERRO] Erro a abrir o FileMapping.\n"));
		CloseHandle(hMapFile);
		ExitProcess(-1);
	}
	pdata->controlingData.hMapFile = hMapFile;
	pdata->controlingData.sharedMem = (SharedMem*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pdata->controlingData.sharedMem == NULL) {
		_tprintf(TEXT("[ERRO] Alocação de memória deu erro\n"));
		CloseHandle(hMapFile);
		ExitProcess(-1);
	}
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENTSHAREDMEM);
	if (hEvent == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possível aceder ao evento desejado.\n"));
		CloseHandle(hEvent);
		ExitProcess(1);
	}

	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEXSHAREDMEM);
	if (hMutex == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possível abrir o Mutex.\n"));
		CloseHandle(hMutex);
		ExitProcess(1);
	}
	pdata->controlingData.hEvent = hEvent;
	pdata->controlingData.hMutex = hMutex;
	TCHAR localBG[10][20];
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD pos;
	pos.X = 0;
	pos.Y = 5;
	DWORD flag = 0;
	HANDLE hEventos[2];
	hEventos[0] = pdata->controlingData.hEvent;
	hEventos[1] = OpenEvent(EVENT_ALL_ACCESS, FALSE, TERMINOUJOGO);
	const char* message = "FECHA";
	do {
		DWORD temp = WaitForMultipleObjects(2,hEventos, FALSE, INFINITE);
		if (temp == WAIT_OBJECT_0 + 1) {
			_tprintf_s(TEXT("O servidor encerrou o jogo!\n"));
			break;
		}
		
		temp = WaitForSingleObject(pdata->controlingData.hMutex, 10000);
		if (temp == WAIT_TIMEOUT) {
			_tprintf_s(TEXT("\nServidor foi encerrado!\n"));
			flag = -1;
			break;
		}
		CopyMemory(&localBG, pdata->controlingData.sharedMem->gameShared, sizeof(pdata->controlingData.sharedMem->gameShared));
		if (!ReleaseMutex(pdata->controlingData.hMutex)) break;
		ResetEvent(pdata->controlingData.hEvent);
		WaitForSingleObject(pdata->hMutex, INFINITE);
		GetConsoleScreenBufferInfo(pdata->hStdout, &csbi);
		SetConsoleCursorPosition(pdata->hStdout, pos);
		_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
		showBG(localBG, pdata->controlingData.sharedMem->faixaMax);
		SetConsoleCursorPosition(pdata->hStdout, csbi.dwCursorPosition);
		if (!ReleaseMutex(pdata->hMutex)) break;
		Sleep(500);
	} while (!pdata->controlingData.fechar);

	UnmapViewOfFile(pdata->controlingData.sharedMem);
	CloseHandle(hMapFile);
	CloseHandle(hEvent);
	CloseHandle(hMutex);
	CloseHandle(pdata->hMutex);
	ExitThread(1);
}