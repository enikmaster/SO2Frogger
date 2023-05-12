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


typedef struct FAIXAVELOCITY FaixaVelocity;
typedef struct POS pos;
typedef struct BOARDGAME BoardGame;
typedef struct OBJECTO objs;
typedef struct INFO Info;
typedef struct _BUFFERCELL BufferCell;
typedef struct _SHAREDMEM SharedMem;
typedef struct _CONTROLDATA ControlData;

typedef struct infoextra {
	HANDLE hStdout;
	HANDLE hMutex;
}infoextra;

#endif