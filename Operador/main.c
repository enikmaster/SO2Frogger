#include "operador.h"


typedef struct InfoToThread {
	COORD x;
	HANDLE hStdout;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
}InfoToThread;

DWORD WINAPI GetInput(LPVOID param) {
	InfoToThread* pdata = (InfoToThread*)param;
	pdata->x.X = 0;
	pdata->x.Y = 45;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(pdata->hStdout, &csbi);

	SetConsoleCursorPosition(pdata->hStdout, pdata->x);
	TCHAR str[100];
	_fgetts(str, sizeof(str) / sizeof(TCHAR), stdin);

	SetConsoleCursorPosition(pdata->hStdout, csbi.dwCursorPosition);
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
	_tprintf_s(TEXT("Operador")); // usar TEXT assegura que a string está codificada devidamente
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Error: CreateFileMapping (%d)\n"), GetLastError());
		return FALSE;
	}
	ControlData dados;
	dados.sharedMem = (SharedMem*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (dados.sharedMem == NULL) {
		_tprintf(TEXT("Error: Alocação de memória deu erro\n"));
	}

	COORD posI = { 0,4 };
	DWORD res;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	BOOL succeed = FillConsoleOutputCharacter(hStdout, _T(' '), 70 * 70, posI, &res);
	
	SetConsoleCursorPosition(hStdout, posI);
	
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENTSHAREDMEM);
	if (hEvent == NULL) {
		ExitProcess(1);
	}

	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEXSHAREDMEM);
	
	TCHAR array[10][20];
	
	InfoToThread a;
	a.hStdout = hStdout;
	a.x = posI;
	
	HANDLE hThread = CreateThread(NULL, 0, GetInput, &a, 0, NULL);

	while (1) {
		WaitForSingleObject(hEvent, INFINITE);
		WaitForSingleObject(hMutex, INFINITE);

		CopyMemory(&array, dados.sharedMem->array, sizeof(dados.sharedMem->array));

		ReleaseMutex(hMutex);
		ResetEvent(hEvent);

		posI.X = 0;
		posI.Y = 7;
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hStdout, &csbi);

		CONSOLE_CURSOR_INFO cursorInfo;
		SetConsoleCursorInfo(hStdout, &cursorInfo);
		cursorInfo.dwSize = 1;
		cursorInfo.bVisible = FALSE;

		/* -> Possivel mas não funciona corretamente
		_tprintf_s(TEXT("-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
		for (int i = 0; i < 10; i++) {
			TCHAR s[21];
			_tcsncpy_s(s, sizeof(s), array[i], _TRUNCATE);
			s[20] = '\0'; // Null-terminate the buffer
			_tprintf_s(TEXT("     %s     "), s);
			_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    -  \n"));
		}
		*/ 
		_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
		for (int i = 0; i < 10; i++) {

			if (dados.sharedMem->array == NULL)
				_tprintf_s(TEXT(" NULO "));
			for (int j = 0; j < 20; j++) {
				_tprintf_s(TEXT(" %c "), array[i][j]); //copyMemory(estrutura
			}
			_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    -  \n"));

		}
		SetConsoleCursorInfo(hStdout, &cursorInfo);
		SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);
	}
	CloseHandle(hMutex);
	CloseHandle(hMapFile);
	return 0;
}
