#include "servidor.h"
#include "..\Froggerino\froggerino.h" // funções da DLL

//vamos ter uma função para fechar tudo

int _tmain(int argc, TCHAR** argv) {

#ifdef UNICODE
	DWORD x1, x2, x3;
	x1 = _setmode(_fileno(stdin), _O_WTEXT);
	x2 = _setmode(_fileno(stdout), _O_WTEXT);
	x3 = _setmode(_fileno(stderr), _O_WTEXT);
	if (x1 == -1 || x2 == -1 || x3 == -1)
		ExitProcess(-1);
#endif

	srand(time(NULL));
	
	COORD posI = { 0,0 };
	DWORD res;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdout == NULL) {
		_tprintf_s(TEXT("[ERRO] Erro"));
		ExitProcess(-1);
	}
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	posI.X = 0;
	posI.Y = 0;
	BOOL succeed = FillConsoleOutputCharacter(hStdout, _T(' '), 50 * 50, posI, &res);
	if (!succeed) {
		_tprintf_s(TEXT("[ERRO] Error"));
		ExitProcess(-1);
	}
	posI.X = 0;
	posI.Y = 0;
	SetConsoleCursorPosition(hStdout, posI);
	
	// verifica se o mutex foi criado com sucesso
	ControlData a;

	HANDLE hMutex = checkStart();
	if (hMutex == NULL) {
		CloseHandle(hMutex);
		ExitProcess(0);
	}
	FaixaVelocity dados;

	// verifica os dados recebidos
	checkArgs(argc - 1, argv, &dados);
	// lanca a thread
	lancaThread(dados, posI, hStdout);
	CloseHandle(hMutex);
	CloseHandle(hStdout);

	return 0;
}