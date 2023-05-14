#ifndef SERVIDOR_H
#define SERVIDOR_H

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
#define SHM_NAME TEXT("SHM_PC")
#define MUTEX_NAME TEXT("MUTEXP")
#define SEM_WRITE_NAME TEXT("SEM_WRITE")
#define SEM_READ_NAME TEXT("SEM_READ")
#define EVENTSHAREDMEM TEXT("EVENTOREAD")
#define MUTEXSHAREDMEM TEXT("HMUTEX")
#define TAMWORD 512
#define TAMARGSFAIXA 6
#define WINDOWCOLUMNS 80 
#define WINDOWROW 26
#define YBOARD 18
#define YINPUTS 1
#define BUFFER_SIZE 10

 typedef enum {
	CMD_PARAR,    // uma palavra + um inteiro > 0 && < total de faixas
	CMD_INVERTER, // uma palavra + um inteiro > 0 && < total de faixas
	CMD_ADICIONAR, // uma palavra + um inteiro > 0 && < total de faixas
	CMD_RETOMAR,
	CMD_SAIR,
	CMD_PAUSEALL,
	CMD_ERRO // comando inválido
} Comando;

// estruturas a usar entre os vários programas
 typedef struct FAIXAVELOCITY { // informação sobre a faixa
	DWORD faixa;
	DWORD velocity;
} FaixaVelocity;

 typedef struct POS { //posição para array, cada thread trada de escrever uma faixa
	DWORD X;
	DWORD Y;
} pos;

 typedef struct BOARDGAME { // tabuleiro de jogo
	TCHAR** gameBoardArray;
	pos d;
	HANDLE hMutexArray;
	HANDLE hTimerBoard;
} BoardGame;

typedef struct OBJECTO { // objetos do tabuleiro
	TCHAR s; // sapo 
	TCHAR c; // carro
	TCHAR o; // objeto
} objs;

 typedef struct INFO { // Informação completa sobre o jogo
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
	unsigned int score; // pontuação
	unsigned int time;  // tempo
} Info;

 typedef struct _BUFFERCELL { // informação para ser lida pelo servidor
	unsigned int id;
	DWORD f1;
	DWORD f2;
} BufferCell;

 typedef struct _SHAREDMEM { // memória partilhada
	unsigned int faixaMax;
	unsigned int p;
	unsigned int c;
	unsigned int wP; //posição escrita
	unsigned int rP; //posição read
	BufferCell buffer[BUFFER_SIZE];
	TCHAR gameShared[10][20];
} SharedMem;
//typedef struct _CONTROLDATA ControlData;

typedef struct _CONTROLDATA { // informação para controlo de fluxo de dados
	unsigned int id;
	HANDLE hMapFile;
	SharedMem* sharedMem;
	HANDLE hMutex;
	HANDLE hEvent;
	HANDLE hWriteSem; // n
	HANDLE hReadSem;  // 1
	BOOL fechar;
} ControlData;

typedef struct infoextra {
	HANDLE hStdout;
	HANDLE hMutex;
	ControlData controlingData;
}infoextra;

//// Funcoes
void showBG(TCHAR localBG[10][20], DWORD TamFaixa);
DWORD WINAPI GetInput(LPVOID param);
Comando checkInput(TCHAR* msg, infoextra* info, DWORD* AltFaixa, DWORD maxFaixas);

#endif