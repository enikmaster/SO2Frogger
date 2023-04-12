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
#define key1is "Software\\MyApp\\nFaixas"
#define key2is "Software\\MyApp\\Velocidade"

//
void checkStart(); //verificar se programa tem condições de ser executado
void checkArgs(int x, char* args); //verificar argumentos
int checkIfNumero(char* arg1, char* arg2); //verificar se args sao numeros

#endif