#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#define programa "servidor"
#define TAM 50
#define key1is "Software\\TP"
#define keyValueNameF "Faixas" 
#define keyValueNameV "Velocity"
#define COLUMNS 20
#define JANELAX 60 
#define JANELAY 40
#define BUFFER_SIZE 10
#define SHM_NAME TEXT("SHM_PC")
#define MUTEX_NAME TEXT("MUTEXP")
#define SEM_WRITE_NAME TEXT("SEM_WRITE")
#define SEM_READ_NAME TEXT("SEM_READ")

typedef struct faixaVelocity {
	DWORD faixa;
	DWORD velocity;
} FaixaVelocity;

typedef struct POS { //posi��o para array, cada thread trada de escrever uma faixa
	DWORD X;
	DWORD Y;
}pos;

typedef struct BoardGame {
	TCHAR** gameBoardArray;
	pos d;
	HANDLE hMutexArray, hTimerBoard;
}BoardGame;

typedef struct objectos {
	TCHAR s; // sapo 
	TCHAR c; // carro
	TCHAR o; // objeto
} objs;

//tenho de escrever 

typedef struct Info {
	TCHAR** arrayGame;
	DWORD veloc; //velocidade 
	DWORD nFaixaResp; //faixa de atuacao
	DWORD id; //id
	DWORD end;
	HANDLE hMutexArray, hTimer, hStdout;
	objs o; //objetos
	// TODO:
	// adicionar pontua��o
	// tempo
}Info;


/////////////
typedef struct _BufferCell {
	unsigned int id;
	TCHAR val[100];
} BufferCell;

typedef struct _SharedMem {
	unsigned int p;
	unsigned int c;
	unsigned int wP;
	unsigned int rP;
	BufferCell buffer[BUFFER_SIZE];
	TCHAR array[10][20];
} SharedMem;

typedef struct _ControlData {
	// unsigned int shutdown;
	unsigned int id;
	HANDLE hMapFile;
	SharedMem* sharedMem;
	HANDLE hMutex;
	HANDLE hWriteSem; // n
	HANDLE hReadSem;  // 1
} ControlData;

int _tmain(int argc, TCHAR** argv) {
#ifdef UNICODE
	DWORD x1, x2, x3;
	x1 = _setmode(_fileno(stdin), _O_WTEXT);
	x2 = _setmode(_fileno(stdout), _O_WTEXT);
	x3 = _setmode(_fileno(stderr), _O_WTEXT);
	if (x1 == -1 || x2 == -1 || x3 == -1)
		ExitProcess(-1);
#endif
	_tprintf_s(TEXT("Operador")); // usar TEXT assegura que a string est� codificada devidamente
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Error: CreateFileMapping (%d)\n"), GetLastError());
		return FALSE;
	}
	ControlData dados;
	dados.sharedMem = (SharedMem*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (dados.sharedMem == NULL) {
		_tprintf(TEXT("Error: Aloca��o de mem�ria deu erro\n"));
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
				_tprintf_s(TEXT(" %c "), dados.sharedMem->array[i][j]);
			}
			_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    -  \n"));

		}
		SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);
	}
	CloseHandle(hMapFile);
	return 0;
}
