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
#include "..\Froggerino\froggerino.h"

typedef struct FAIXAVELOCITY FaixaVelocity;
typedef struct POS pos;
typedef struct BOARDGAME BoardGame;
typedef struct OBJECTO objs;
typedef struct INFO Info;
typedef struct _BUFFERCELL BufferCell;
typedef struct _SHAREDMEM SharedMem;
typedef struct SAPO SAPO;
typedef struct Eventos_Mutexs Eventos_Mutexs;;
typedef struct ControlaPipes ControlaPipes;
typedef struct NIVEL Nivel;
typedef struct _CONTROLDATA ControlData;

HANDLE checkStart(); //verificar se programa tem condições de ser executado
void checkArgs(int x, TCHAR** args, FaixaVelocity* dados); //verificar argumentos
int checkIfNumero(char* arg1, char* arg2); //verificar se args sao numeros
int criarRegKeys(int arg1, int arg2); //criar RegKeys
void setDadosEstrutura(FaixaVelocity* dados); // preenche a estrutura com os dados para Faixa e Velocity
void DisconnectAndReconnect(DWORD);
BOOL ConnectToNewClient(HANDLE, LPOVERLAPPED);
void GetAnswerToRequest(LPPIPEINST);
// threads
void lancaThread(FaixaVelocity dados, COORD posI, HANDLE hStdout);
DWORD WINAPI ThreadsFaixa(LPVOID param);
DWORD WINAPI EstadoTabuleiro(LPVOID param);
DWORD WINAPI LeComandosOperadoresThread(LPVOID param);
DWORD WINAPI ThreadVeTeclado(LPVOID param);
DWORD WINAPI ControlaPipesF(LPVOID param);
DWORD WINAPI ThreadsParaSapo(LPVOID param);
DWORD WINAPI ThreadTempo(LPVOID param);
//
void DesligaELiga(DWORD);
BOOL ConnectNovoCliente(HANDLE, LPOVERLAPPED);
void ReceberResposta(LPPIPEINST);
void PreencheSapos(SAPO* sapos, const int x);
void PreencheNiveis(Nivel* niveis, SAPO* sapos);



// shared memory
BOOL initMemAndSync(ControlData* cData, Info* dados, DWORD x);



#endif