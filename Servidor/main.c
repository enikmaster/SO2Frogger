#include "servidor.h"

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
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	posI.X = 0;
	posI.Y = 0;
	BOOL succeed = FillConsoleOutputCharacter(hStdout, _T(' '), 50 * 50, posI, &res);

	if (!succeed) {
		_tprintf_s(TEXT("Não reune condições de continuar!\n"));
		ExitProcess(-1);
	}
	posI.X = 0;
	posI.Y = 0;
	SetConsoleCursorPosition(hStdout, posI);
	
	HANDLE hMutex = checkStart();
	FaixaVelocity dados;
	checkArgs(argc - 1, argv, &dados);
	lancaThread(dados, posI, hStdout);
	while (1) {
		TCHAR hi[20];
		_fgetts(hi, 20, stdin);
		CloseHandle(hMutex);
		return 0;
	}

	return 0;
}