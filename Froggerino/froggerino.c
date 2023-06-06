// froggerino.cpp - definição das funções exportadas para o DLL
#include "pch.h" // obrigatório
// outros includes necessários
#include "framework.h"
#include "froggerino.h" // obrigatório
#include <tchar.h>

// Variáveis internas da DLL caso haja alguma

// Definição de funções
//// funções para ler das estruturas (getters)
//// Info
 

//// funções para escrever nas estruturas (setters)
FROGGERINO_API void teste() {
	_tprintf_s(TEXT("DLL a funcionar\n"));
	return;
}

//// outras funções
FROGGERINO_API void showBG(TCHAR localBG[10][20]) {
	for (int i = 0; i < 10; ++i) {
		if (i == 0)
			_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
		if (localBG == NULL)
			_tprintf_s(TEXT(" NULO "));
		for (int j = 0; j < 20; ++j)
			_tprintf_s(TEXT(" %c "), localBG[i][j]);
		_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
	}
}