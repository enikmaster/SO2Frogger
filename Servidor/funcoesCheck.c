#include "servidor.h"
void checkStart() {
	HANDLE hMutex;
	hMutex = CreateMutex(NULL, FALSE, TEXT(programa));
	if (hMutex == NULL) {
		_tprintf_s(TEXT("Erro no handle"));
		ExitProcess(0);
	}
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		_tprintf_s(TEXT("Já existe um servidor a ser executado."));
		CloseHandle(hMutex);
		ExitProcess(0);
	}
	//temos de fechar depois o Mutex;
}
void  checkArgs(int x, char** args) {

	if (x != 2) { // Não recebeu os argumentos

	}
	else if (x == 2) {//
		if (checkIfNumero(args[1], args[2]) == 0) {
			HKEY hKey1, hKey2;
			unsigned dwDisposition1, dwDisposition2;
			LONG ResultKey1, ResultKey2;
		}

	}

}

int checkIfNumero(char* arg1, char* arg2) {
	for (int i = 0; arg1[i] != '\0'; i++)
		if (!isdigit(arg1[i]))
			return -1;
	for (int i = 0; arg2[i] != '\0'; i++)
		if (!isdigit(arg1[i]))
			return -2;

	return 0;
}