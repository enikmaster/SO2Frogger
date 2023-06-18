#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <io.h>
#include "..\Froggerino\froggerino.h"

#define SHM_NAME TEXT("SHM_PC")
#define MUTEX_NAME TEXT("MUTEXP")
#define SEM_WRITE_NAME TEXT("SEM_WRITE")
#define SEM_READ_NAME TEXT("SEM_READ")
#define TAMWORD 512
#define TAMARGSFAIXA 6
#define WINDOWCOLUMNS 80 
#define WINDOWROW 26
#define YBOARD 18
#define YINPUTS 1

typedef struct _CONTROLDATA ControlData;

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