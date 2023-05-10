#include "operador.h"

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

	COORD posI = { 0,0 };
	DWORD res;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	posI.X = 0;
	posI.Y = 0;
	BOOL succeed = FillConsoleOutputCharacter(hStdout, _T(' '), 50 * 50, posI, &res);
	posI.X = 0;
	posI.Y = 0;
	SetConsoleCursorPosition(hStdout, posI);
	while (1) {
		//WaitForevent() // com timeout; MultipleObject caso queira sair 
		
		//Mutexlock;
		//CopyMemory();
		//releaseMutex;
		//TCHAR* s<- faixaResponsavel; 

		posI.X = 0;
		posI.Y = 7;
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hStdout, &csbi);
		SetConsoleCursorPosition(hStdout, posI);

		_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
		for (int i = 0; i < 10; i++) {
			
			if (dados.sharedMem->array == NULL)
				_tprintf_s(TEXT(" NULO "));
			for (int j = 0; j < 20; j++) {
				_tprintf_s(TEXT(" %c "), dados.sharedMem->array[i][j]); //copyMemory(estrutura
			}
			_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    -  \n"));

		}
		SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);
	}
	CloseHandle(hMapFile);
	return 0;
}
