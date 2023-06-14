#include "servidor.h"
#include "..\Froggerino\froggerino.h"

void PreencheSapos(SAPO* x, const int dx) {
	DWORD nRepetePos = rand() % (20);
	for (int i = 0; i < NSAPOS; i++) {
		x[i].move = TRUE;
		x[i].activo = FALSE;
		x[i].vidas = VIDAS;
		x[i].temp = 0;
		x[i].score = 0;
		x[i].pos_atual.X = 0;
		if (i == 0)
			x[i].pos_atual.Y = nRepetePos;
		else {
			do {
				nRepetePos = rand() % (20);
			} while (nRepetePos == x[0].pos_atual.Y);
			x[i].pos_atual.Y = nRepetePos;
		}
		x[i].pos_inicial.X = 0;
		x[i].pos_inicial.Y = x[i].pos_atual.Y;
		x[i].name = TEXT("NULL\0");
	}
}

void PreencheNiveis(Nivel* x, SAPO* sp) {
	x[0].lvlActual = 0;
	for (int i = 0; i < NIVEISDEJOGO; i++) {
		x[i].lvlActual = i+1;
		x[i].sentido = TRUE;
		x[i].nCarros = x[i].lvlActual + 1;
		x[i].velocidade = 1100 - x[i].lvlActual * 100;
		x[i].sapos = sp;
		x[i].tempo = TEMPO - x[i].lvlActual * 1000;
	}
}