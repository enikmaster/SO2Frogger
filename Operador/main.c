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

typedef struct POS { //posição para array, cada thread trada de escrever uma faixa
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
	// adicionar pontuação
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
	Info x;
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






	_tprintf_s(TEXT("-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
	for (int i = 0; i < 10; i++) {
		if (dados.sharedMem->x.arrayGame == NULL)
			_tprintf_s(TEXT(" NULO "));
		else 
			_tprintf_s(TEXT(" NOT NULO "));
		for (int j = 0; j < 20; j++) {
			_tprintf_s(TEXT(" %c "), dados.sharedMem->x.arrayGame[i][j]);
		}
		_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    -  \n"));

	}






	CloseHandle(hMapFile);
	return 0;
}
