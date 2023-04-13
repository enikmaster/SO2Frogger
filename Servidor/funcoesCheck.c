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
	//arg[1] = nº Faixas de Rodagem 
	//arg[2] = Velocidade dos carros inicial
	
	if (x != 2) { // Não recebeu os argumentos

	}

	else if (x == 2) {

		if (checkIfNumero(args[1], args[2]) == 0) {

			if (criarRegKeys(_tstoi(args[1]), _tstoi(args[2]))) {
				_tprintf_s(TEXT("Falha no accesso à informação relativamente às faixas de rodagem ou velocidade inicial dos carros"));
			}
		}
		else {
			_tprintf_s(TEXT("Os argumentos que passou não são válidos, vamos usar uns predefinidos!\n"));
		}

	}

}

int criarRegKeys(int arg1, int arg2) {
	HKEY hKey;
	DWORD disposition;

	LONG ResultKey = RegCreateKeyEx(
		HKEY_CURRENT_USER,
		TEXT(key1is),
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&disposition);

	if (ResultKey != ERROR_SUCCESS) {
		_tprintf_s(TEXT("Erro ao criar/abrir a RegKey\n"));
		RegCloseKey(hKey);
		return -1;
	}

	if (disposition == REG_CREATED_NEW_KEY) {
		//_tprintf_s(TEXT("A chave foi criada.\n"));
	}
	else if (disposition == REG_OPENED_EXISTING_KEY) {
		//_tprintf_s(TEXT("A chave já existe e foi aberta.\n"));
	}

	ResultKey = RegSetValueEx(
		hKey,
		TEXT("Faixas"),
		0,
		REG_DWORD,
		(const BYTE*)&arg1,
		sizeof(arg1));

	if (ResultKey != ERROR_SUCCESS) {
		_tprintf_s(TEXT("Erro ao escrever o valor de nFaixas na RegKey\n"));
		RegCloseKey(hKey);
		return -1;
	}

	ResultKey = RegSetValueEx(
		hKey,
		TEXT("Velocity"),
		0,
		REG_DWORD,
		(const BYTE*)&arg2,
		sizeof(arg2));

	if (ResultKey != ERROR_SUCCESS) {
		_tprintf_s(TEXT("Erro ao escrever o valor Velocity na RegKey\n"));
		RegCloseKey(hKey);
		return -1;
	}
	RegCloseKey(hKey);

	return 0;

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