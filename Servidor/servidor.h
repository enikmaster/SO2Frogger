#ifndef SERVIDOR_H
#define SERVIDOR_H

//Includes
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
#define columns 20
#define JANELAX 60 
#define JANELAY 40

typedef struct faixaVelocity  {
	DWORD faixa;
	DWORD velocity;
} FaixaVelocity;

typedef struct POS { //posição para array, cada thread trada de escrever uma faixa
	DWORD X;
	DWORD Y;
}pos;

typedef struct BoardGame {
	DWORD** gameBoardArray;
	pos d;
	HANDLE hMutexArray, hTimerBoard;
}BoardGame;

typedef struct objectos {
	TCHAR s; // sapo 
	TCHAR c; // carro
	TCHAR o; // objeto
} objs;

typedef struct Info {
	DWORD** arrayGame;
	DWORD veloc; //velocidade 
	DWORD nFaixaResp; //faixa de atuacao
	DWORD id; //id
	DWORD end;
	HANDLE hMutexArray, hTimer, hStdout;
	objs o; //objetos

}Info;

//
HANDLE checkStart(); //verificar se programa tem condições de ser executado
void checkArgs(int x, TCHAR** args, FaixaVelocity* dados); //verificar argumentos
int checkIfNumero(char* arg1, char* arg2); //verificar se args sao numeros
int criarRegKeys(int arg1, int arg2); //criar RegKeys
void setDadosEstrutura(FaixaVelocity* dados); // preenche a estrutura com os dados para Faixa e Velocity

// threads
void lancaThread(FaixaVelocity dados, COORD posI, HANDLE hStdout);


#endif