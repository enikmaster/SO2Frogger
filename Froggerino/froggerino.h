// froggerino.h - cont�m as declara��es das fun��es partilhadas do frogger
#pragma once

#ifdef FROGGERINO_EXPORTS
#define FROGGERINO_API __declspec(dllexport)
#else
#define FROGGERINO_API __declspec(dllimport)
#endif

#define BUFFER_SIZE 10

// estruturas a usar entre os v�rios programas
typedef struct objectos { // objetos do tabuleiro
	TCHAR s; // sapo 
	TCHAR c; // carro
	TCHAR o; // objeto
} objs;

typedef struct Info { // Informa��o completa sobre o jogo
	TCHAR** arrayGame; //aqui feito
	DWORD veloc; //velocidade 
	DWORD nFaixaResp; //faixa de atuacao
	DWORD id; //id
	DWORD end; // end
	HANDLE hMutexArray;
	HANDLE hTimer;
	HANDLE hStdout;
	objs o; //objetos
	// TODO:
	unsigned int score; // pontua��o
	unsigned int time;  // tempo
} Info;

typedef struct _BufferCell { // ainda n�o percebi o que � isto...
	unsigned int id;
	TCHAR val[100];
} BufferCell;

typedef struct _SharedMem { // mem�ria partilhada
	unsigned int p;
	unsigned int c;
	unsigned int wP;
	unsigned int rP;
	BufferCell buffer[BUFFER_SIZE];
	TCHAR gameShared[10][20];
} SharedMem;

typedef struct _ControlData { // informa��o para controlo de fluxo de dados
	unsigned int id;
	HANDLE hMapFile;
	SharedMem* sharedMem;
	HANDLE hMutex;
	HANDLE hWriteSem; // n
	HANDLE hReadSem;  // 1
} ControlData;

// vari�veis
FROGGERINO_API int x;
// FUN��ES
//// fun��es para ler das estruturas (getters)
//// Info

//// fun��es para escrever nas estruturas (setters)

//// outras fun��es
