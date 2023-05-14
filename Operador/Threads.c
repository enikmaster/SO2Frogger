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


	do {
		WaitForSingleObject(pdata->controlingData.hEvent, INFINITE);
		WaitForSingleObject(pdata->controlingData.hMutex, INFINITE);
		CopyMemory(&localBG, pdata->controlingData.sharedMem->gameShared, sizeof(pdata->controlingData.sharedMem->gameShared));
		if (!ReleaseMutex(pdata->controlingData.hMutex)) {
			break;
		}
		ResetEvent(pdata->controlingData.hEvent);
		WaitForSingleObject(pdata->hMutex, INFINITE);
		GetConsoleScreenBufferInfo(pdata->hStdout, &csbi);
		SetConsoleCursorPosition(pdata->hStdout, pos);
		_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
		showBG(localBG, pdata->controlingData.sharedMem->faixaMax);
		SetConsoleCursorPosition(pdata->hStdout, csbi.dwCursorPosition);
		if (!ReleaseMutex(pdata->hMutex)) {
			break;
		}
		Sleep(500);
	} while (!pdata->controlingData.fechar);

	if (!CloseHandle(hMapFile))
		_tprintf_s(TEXT("[ERRO] Erro fechar Handle do hMapFile.\n"));
	if (!CloseHandle(hEvent))
		_tprintf_s(TEXT("[ERRO] Erro fechar Handle do hMapFile.\n"));
	if (!CloseHandle(hMutex))
		_tprintf_s(TEXT("[ERRO] Erro fechar Handle do hMapFile.\n"));
	ExitThread(1);
}