// froggerino.cpp - defini��o das fun��es exportadas para o DLL
#include "pch.h" // obrigat�rio
// outros includes necess�rios
#include "froggerino.h" // obrigat�rio
#include <tchar.h>

// Vari�veis internas da DLL caso haja alguma

// Defini��o de fun��es
//// fun��es para ler das estruturas (getters)
//// Info
 

//// fun��es para escrever nas estruturas (setters)

//// outras fun��es
void showBG(TCHAR localBG[10][20]) {
	for (int i = 0; i < 10; i++) {
		TCHAR lvl0[100] = TEXT("");  
		for (int j = 0; j < 20; j++) {
			TCHAR temp[4];  
			temp[0] = TEXT(' ');
			temp[1] = localBG[i][j]; 
			temp[2] = TEXT(' '); 
			temp[3] = TEXT('\0');  

			wcscat_s(lvl0, 100, temp);  
		}
		_tprintf_s(TEXT("%s\n"), lvl0); 
		_tprintf_s(TEXT("-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
	}
}