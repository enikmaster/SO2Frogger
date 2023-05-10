// froggerino.h - contém as declarações das funções partilhadas do frogger
#pragma once

#ifdef FROGGERINO_EXPORTS
#define FROGGERINO_API __declspec(dllexport)
#else
#define FROGGERINO_API __declspec(dllimport)
#endif

#define BUFFER_SIZE 10

// estruturas a usar entre os vários programas
typedef struct objectos { // objetos do tabuleiro
	TCHAR s; // sapo 
	TCHAR c; // carro
	TCHAR o; // objeto
} objs;

typedef struct Info { // Informação completa sobre o jogo
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
	unsigned int score; // pontuação
	unsigned int time;  // tempo
} Info;

typedef struct _BufferCell { // ainda não percebi o que é isto...
	unsigned int id;
	TCHAR val[100];
} BufferCell;

typedef struct _SharedMem { // memória partilhada
	unsigned int p;
	unsigned int c;
	unsigned int wP;
	unsigned int rP;
	BufferCell buffer[BUFFER_SIZE];
	TCHAR gameShared[10][20];
} SharedMem;

typedef struct _ControlData { // informação para controlo de fluxo de dados
	unsigned int id;
	HANDLE hMapFile;
	SharedMem* sharedMem;
	HANDLE hMutex;
	HANDLE hWriteSem; // n
	HANDLE hReadSem;  // 1
} ControlData;

// variáveis
FROGGERINO_API int x;
// FUNÇÕES
//// funções para ler das estruturas (getters)
//// Info

//// funções para escrever nas estruturas (setters)

//// outras funções
