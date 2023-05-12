#include "operador.h"
#include "..\Froggerino\froggerino.h"

typedef struct INFO_TO_THREAD {
	COORD x;
	HANDLE hStdout;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
} InfoToThread;

void showBG(TCHAR localBG[10][20]) {
	for (int i = 0; i < 10; i++) {
		TCHAR lvl0[100] = TEXT("");  

		for (int j = 0; j < 20; j++) {
			TCHAR temp[4];  
			temp[0] = TEXT(' ');
			temp[1] = localBG[i][j]; 
			temp[2] = TEXT(' '); 
			temp[3] = TEXT('\0');  

			wcscat_s(lvl0, 100, temp);  // Concatenate the temporary string to lvl0
		}
		_tprintf_s(TEXT("%s\n"), lvl0);  // Print the row
		_tprintf_s(TEXT("-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
	} 
	/*
	for (int i = 0; i < 10; ++i) {
		if (localBG == NULL)
			_tprintf_s(TEXT(" NULO "));
		for (int j = 0; j < 20; ++j)
			_tprintf_s(TEXT(" %c "), localBG[i][j]);
		_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
	}
	*/
}

DWORD WINAPI GetInput(LPVOID param) {
	infoextra* pdata = (infoextra*)param;
	ControlData dados;
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Error: CreateFileMapping (%d)\n"), GetLastError());
		return FALSE;
	}
	dados.hMapFile = hMapFile;
	dados.sharedMem = (SharedMem*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (dados.sharedMem == NULL) {
		_tprintf(TEXT("Error: Alocação de memória deu erro\n"));
	}
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENTSHAREDMEM);
	if (hEvent == NULL) {
		ExitProcess(1);
	}

	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEXSHAREDMEM);
	dados.hEvent = hEvent;
	dados.hMutex = hMutex;
	TCHAR localBG[10][20];
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD pos;
	pos.X = 0;
	pos.Y = 5;
	do {
		WaitForSingleObject(dados.hMutex, INFINITE);
		CopyMemory(&localBG, dados.sharedMem->gameShared, sizeof(dados.sharedMem->gameShared));
		ReleaseMutex(dados.hMutex);
		ResetEvent(dados.hEvent);
		WaitForSingleObject(pdata->hMutex, INFINITE);
		GetConsoleScreenBufferInfo(pdata->hStdout, &csbi);
		SetConsoleCursorPosition(pdata->hStdout, pos);
		_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
		showBG(localBG);
		SetConsoleCursorPosition(pdata->hStdout, csbi.dwCursorPosition);
		ReleaseMutex(pdata->hMutex);
		Sleep(500);
	} while (1);
	
	CloseHandle(dados.hMapFile);
	ExitThread(1);
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
	_tprintf_s(TEXT("Operador")); // usar TEXT assegura que a string está codificada devidamente

	
	HANDLE MutexShared = CreateMutex(NULL, FALSE, TEXT("MUTEXLOCAL"));

	
	infoextra extra;
	extra.hMutex = MutexShared;
	extra.hStdout = hStdout;
	
	HANDLE hThread = CreateThread(NULL, 0, GetInput, &extra, 0, NULL);
	int count = 0;
	TCHAR name[256];
	while (1) {
		DWORD written;
		CONSOLE_SCREEN_BUFFER_INFO info;

		pos.X = 0;
		pos.Y = 1;
		SetConsoleCursorPosition(hStdout, pos);
		WaitForSingleObject(extra.hMutex, INFINITE);
		_tprintf_s(TEXT("Comando a enviar: "));
		ReleaseMutex(extra.hMutex);
		pos.X = 18;
		pos.Y = 1;
		SetConsoleCursorPosition(hStdout, pos);
		_fgetts(name, sizeof(name) / sizeof(TCHAR), stdin);
		WaitForSingleObject(extra.hMutex, INFINITE);
		if (count > 0) {
			pos.X = 0;
			pos.Y = 2;
			SetConsoleCursorPosition(hStdout, pos);
			FillConsoleOutputCharacter(hStdout, ' ', 50, pos, &written);
		}
		_tprintf_s(TEXT("Você enviou: %s"), name);
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
