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

typedef struct faixaVelocity  {
	DWORD faixa;
	DWORD velocity;
} FaixaVelocity;

//
HANDLE checkStart(); //verificar se programa tem condições de ser executado
void checkArgs(int x, TCHAR** args, FaixaVelocity* dados); //verificar argumentos
int checkIfNumero(char* arg1, char* arg2); //verificar se args sao numeros
int criarRegKeys(int arg1, int arg2); //criar RegKeys
void setDadosEstrutura(FaixaVelocity* dados); // preenche a estrutura com os dados para Faixa e Velocity
#endif