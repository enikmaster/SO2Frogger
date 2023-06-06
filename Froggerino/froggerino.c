// froggerino.cpp - defini��o das fun��es exportadas para o DLL
#include "pch.h" // obrigat�rio
// outros includes necess�rios
#include "framework.h"
#include "froggerino.h" // obrigat�rio
#include <tchar.h>

// Vari�veis internas da DLL caso haja alguma

// Defini��o de fun��es
//// fun��es para ler das estruturas (getters)
//// Info
 

//// fun��es para escrever nas estruturas (setters)
FROGGERINO_API void teste() {
	_tprintf_s(TEXT("DLL a funcionar\n"));
	return;
}

//// outras fun��es
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