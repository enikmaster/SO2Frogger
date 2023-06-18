#pragma once
#include <windows.h>
#include <tchar.h>
//#include <math.h>
//#include <stdio.h>
//#include <fcntl.h>
//#include <io.h>
#include <windowsx.h>

#define PIPE_NAME TEXT("\\\\.\\pipe\\canal")
#define MAX_MESSAGE_SIZE 1024
#define IDM_SINGLE_PLAYER 1001
#define IDM_MULTI_PLAYER 1002
#define IDM_ABOUT 1003
#define IDM_EXIT 1004
#define IDM_COR_LIGHT 1005
#define IDM_COR_DARK 1006
#define BUFSIZE 4096
#define NUMERO_COLUNAS 20
#define SINGLEPLAYEROPT TEXT("1\0")
#define MOVE_RIGHT TEXT("3\0")
#define MOVE_LEFT TEXT("4\0")
#define MOVE_UP TEXT("5\0")
#define MOVE_DOWN TEXT("6\0")
#define IDM_PAUSE_GAME 1007
#define IDM_RESUME_GAME 1008
#define IDM_RESTART_GAME 1009

#define SAPO TEXT('S')
#define CARRO TEXT('C')
#define OBSTACULO TEXT('O')
#define DESLOCAMENTO 36

typedef struct OBJETO {
	int posicaoX;
	int posicaoY;
	TCHAR tipo;
};
// estrutura de dados locais do jogo
typedef struct LOCAL {
	int nivel;
	int vidas;
	int tempo;
	int pontuacao;
	int numeroFaixas;
	DWORD nBytesRead;
	DWORD nBytesWriten;
	TCHAR mensagem[BUFSIZE];
	TCHAR mensagemaEnviar[2];
	OBJETO objetos[200];
	HANDLE hPipe;
	HWND hWnd;
	CRITICAL_SECTION cs;
	HANDLE hEventoEnviaMensagem;
	DWORD jumpSucesso;
	DWORD myX, myY;
	BOOL playing = FALSE;
};

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);
void ParseMessage(LOCAL* origenate);
DWORD WINAPI lerMessages(LPVOID param);


