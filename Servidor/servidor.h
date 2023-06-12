#ifndef SERVIDOR_H
#define SERVIDOR_H

//Includes
#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <io.h>

#define programa "servidor"
#define TAM 50
#define key1is "Software\\TP"
#define NOMEPIPE TEXT("\\\\.\\pipe\\canal")
#define keyValueNameF "Faixas" 
#define keyValueNameV "Velocity"
#define COLUMNS 20
#define JANELAX 60 
#define JANELAY 40
#define BUFFER_SIZE 10
#define EVENTSHAREDMEM TEXT("EVENTOREAD")
#define MUTEXSHAREDMEM TEXT("HMUTEX")
#define CONNECTING_STATE 0 
#define READING_STATE 1 
#define WRITING_STATE 2 
#define INSTANCES 2
#define PIPE_TIMEOUT 5000
#define BUFSIZE 4096

typedef struct FAIXAVELOCITY FaixaVelocity;
typedef struct POS pos;
typedef struct BOARDGAME BoardGame;
typedef struct OBJECTO objs;
typedef struct INFO Info;
typedef struct _BUFFERCELL BufferCell;
typedef struct _SHAREDMEM SharedMem;
//typedef struct _CONTROLDATA ControlData;
typedef struct _CONTROLDATA { // informação para controlo de fluxo de dados
	unsigned int id;
	HANDLE hMapFile;
	SharedMem* sharedMem;
	HANDLE hMutex;
	HANDLE hEvent;
	HANDLE hWriteSem; // n
	HANDLE hReadSem;  // 1
	Info* infoControl;
} ControlData;
// 
//Para gestao de sapo a mover e ainda a informacao 





//
HANDLE checkStart(); //verificar se programa tem condições de ser executado
void checkArgs(int x, TCHAR** args, FaixaVelocity* dados); //verificar argumentos
int checkIfNumero(char* arg1, char* arg2); //verificar se args sao numeros
int criarRegKeys(int arg1, int arg2); //criar RegKeys
void setDadosEstrutura(FaixaVelocity* dados); // preenche a estrutura com os dados para Faixa e Velocity
VOID DisconnectAndReconnect(DWORD);
BOOL ConnectToNewClient(HANDLE, LPOVERLAPPED);
VOID GetAnswerToRequest(LPPIPEINST);
// threads
void lancaThread(FaixaVelocity dados, COORD posI, HANDLE hStdout);

//
VOID DesligaELiga(DWORD);
BOOL ConnectNovoCliente(HANDLE, LPOVERLAPPED);
VOID ReceberResposta(LPPIPEINST);



// shared memory
BOOL initMemAndSync(ControlData* cData, Info* dados, DWORD x);



#endif