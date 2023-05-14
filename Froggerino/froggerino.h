// froggerino.h - cont�m as declara��es das fun��es partilhadas do frogger
#pragma once

#ifdef FROGGERINO_EXPORTS
#define FROGGERINO_API __declspec(dllexport)
#else
#define FROGGERINO_API __declspec(dllimport)
#endif

#define BUFFER_SIZE 10


FROGGERINO_API typedef enum {
	CMD_PARAR,    // uma palavra + um inteiro > 0 && < total de faixas
	CMD_INVERTER, // uma palavra + um inteiro > 0 && < total de faixas
	CMD_ADICIONAR, // uma palavra + um inteiro > 0 && < total de faixas
	CMD_RETOMAR,
	CMD_SAIR,
	CMD_ERRO // comando inv�lido
} Comando;

// estruturas a usar entre os v�rios programas
FROGGERINO_API typedef struct FAIXAVELOCITY { // informa��o sobre a faixa
	DWORD faixa;
	DWORD velocity;
} FaixaVelocity;

FROGGERINO_API typedef struct POS { //posi��o para array, cada thread trada de escrever uma faixa
	DWORD X;
	DWORD Y;
} pos;

FROGGERINO_API typedef struct BOARDGAME { // tabuleiro de jogo
	TCHAR** gameBoardArray;
	pos d;
	HANDLE hMutexArray;
	HANDLE hTimerBoard;
} BoardGame;

FROGGERINO_API typedef struct OBJECTO { // objetos do tabuleiro
	TCHAR s; // sapo 
	TCHAR c; // carro
	TCHAR o; // objeto
} objs;

FROGGERINO_API typedef struct INFO { // Informa��o completa sobre o jogo
	TCHAR** arrayGame; //aqui feito
	DWORD veloc; //velocidade 
	DWORD nFaixaResp; //faixa de atuacao
	DWORD id; //id
	DWORD end; // end
	DWORD sentido; //if 1 direita if 0 esquerda
	BOOL moving; //if true andar if false parar
	HANDLE hMutexArray;
	HANDLE hEventStart;
	HANDLE hEventPause;
	HANDLE hStdout;
	objs o; //objetos
	// TODO:
	unsigned int score; // pontua��o
	unsigned int time;  // tempo
} Info;

FROGGERINO_API typedef struct _BUFFERCELL { // informa��o para ser lida pelo servidor
	unsigned int id;
	DWORD f1;
	DWORD f2;
} BufferCell;

FROGGERINO_API typedef struct _SHAREDMEM { // mem�ria partilhada
	unsigned int faixaMax;
	unsigned int p;
	unsigned int c;
	unsigned int wP; //posi��o escrita
	unsigned int rP; //posi��o read
	BufferCell buffer[BUFFER_SIZE];
	TCHAR gameShared[10][20];
} SharedMem;

//FROGGERINO_API typedef struct _CONTROLDATA { // informa��o para controlo de fluxo de dados
//	unsigned int id;
//	HANDLE hMapFile;
//	SharedMem* sharedMem;
//	HANDLE hMutex;
//	HANDLE hEvent;
//	HANDLE hWriteSem; // n
//	HANDLE hReadSem;  // 1
//} ControlData;

// vari�veis
FROGGERINO_API int x;
// FUN��ES
//// fun��es para ler das estruturas (getters)
//// Info

//// fun��es para escrever nas estruturas (setters)

//// outras fun��es
#ifdef __cplusplus
extern "C" {
#endif

	//FROGGERINO_API void showBG(TCHAR);

#ifdef __cplusplus
}
#endif