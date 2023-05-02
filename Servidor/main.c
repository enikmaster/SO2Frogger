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
	HANDLE hMutex = checkStart();
	FaixaVelocity dados;
	checkArgs(argc - 1, argv, &dados);

	COORD posI = { 0,0 };
	DWORD res;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	BOOL succeed = FillConsoleOutputCharacter(hStdout, _T(' '), JANELAX * JANELAY, posI, &res);
	
	if (!succeed) {
		_tprintf("Não reune as condições");
		ExitProcess(1);
	}
	//lancaThread(dados, posI, hStdout);
	while (1) {
		TCHAR hi[20];
		_fgetts(hi, 20, stdin);
		CloseHandle(hMutex);
		return 0;
	}

	return 0;
}