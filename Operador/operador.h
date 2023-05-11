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
#define BUFFER_SIZE 10
#define SHM_NAME TEXT("SHM_PC")
#define MUTEX_NAME TEXT("MUTEXP")
#define SEM_WRITE_NAME TEXT("SEM_WRITE")
#define SEM_READ_NAME TEXT("SEM_READ")
#define EVENTSHAREDMEM TEXT("EVENTOREAD")
#define MUTEXSHAREDMEM TEXT("HMUTEX")


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
	TCHAR array[10][20];
} SharedMem;

typedef struct _ControlData {
	// unsigned int shutdown;
	unsigned int id;
	HANDLE hMapFile;
	SharedMem* sharedMem;
	HANDLE hMutex;
	HANDLE Event;
	HANDLE hWriteSem; // n
	HANDLE hReadSem;  // 1
} ControlData;

#endif