#include "servidor.h"
#include "..\Froggerino\froggerino.h"

// verifica se mutex foi criado com sucesso
HANDLE checkStart() {
	HANDLE hMutex = CreateMutex(NULL, FALSE, TEXT(programa));
	if (hMutex == NULL) {
		_tprintf_s(TEXT("Erro no handle"));
		ExitProcess(0);
	}
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		_tprintf_s(TEXT("Já existe um servidor a ser executado."));
		CloseHandle(hMutex);
		ExitProcess(0);
	}
	return hMutex;
}

// verifica os dados recebidos
void checkArgs(int x, char** args, FaixaVelocity* dados) {
	//args[1] = nº Faixas de Rodagem com MAXIMO DE 10 
	//args[2] = Velocidade dos carros inicial  
	if (x == 0) // Não recebeu os argumentos, usar valores predefenidos
		setDadosEstrutura(dados);
	else if (x == 2) {
		if (checkIfNumero(args[1], args[2]) == 0 && checkIfValidNumber(args[1], args[2]) == 0) {
			// verificar se são valores válidos (positivos, entre x e y)
			if (criarRegKeys(_tstoi(args[1]), _tstoi(args[2]))) {
				_tprintf_s(TEXT("Falha no accesso à informação relativamente às faixas de rodagem ou velocidade inicial dos carros"));
				return -1;
			}
			dados->faixa = _tstoi(args[1]);
			dados->velocity = _tstoi(args[2]);
			//_tprintf_s(TEXT("%d %d"), dados->faixa, dados->velocity);
		}
		else {
			_tprintf_s(TEXT("Os argumentos que passou não são válidos, vamos usar uns predefinidos!\n"));
			setDadosEstrutura(dados);
		}
	}
	else if (x == 1 || x > 2) // número de argumentos inválido
		setDadosEstrutura(dados); // para já, usa-se os valor default
}

// crias as HKeys
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
		//return -2;
	}

	ResultKey = RegSetValueEx(
		hKey,
		TEXT(keyValueNameF),
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
		TEXT(keyValueNameV),
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

// verifica se os dados inseridos são números
int checkIfNumero(char* arg1, char* arg2) {
	for (int i = 0; arg1[i] != '\0'; i++)
		if (!isdigit(arg1[i]))
			return -1;
	for (int i = 0; arg2[i] != '\0'; i++)
		if (!isdigit(arg1[i]))
			return -2;

	return 0;
}

// verifica se os números são válidos
int checkIfValidNumber(char* arg1, char* arg2) {
	// verifica se são positivos
	if (_tstoi(arg1) <= 0 || _tstoi(arg2) <= 0)
		return -1;
	// falta verificar se estão entre x e y
	return 0;
}

// coloca os dados recebidos numa HKey
void setDadosEstrutura(FaixaVelocity* dados) {
	DWORD size = sizeof(DWORD);
	HKEY temp;
	DWORD disposition;

	LONG ResultKey = RegCreateKeyEx(
		HKEY_CURRENT_USER,
		TEXT(key1is),
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&temp,
		&disposition);

	if (disposition == REG_OPENED_EXISTING_KEY) {
		// estes if só são necessários para verificações de sucesso
		// se não for necessário podem ser substyituídos por:
		// RegQueryValueExA(temp, keyValueNameF, NULL, NULL, (LPBYTE)&dados->faixa, &size)
		// RegQueryValueExA(temp, keyValueNameV, NULL, NULL, (LPBYTE)&dados->velocity, &size)
		if (RegQueryValueExA(temp, keyValueNameF, NULL, NULL, (LPBYTE)&dados->faixa, &size) == ERROR_SUCCESS)
			_tprintf_s(TEXT("Faixas: %d\n"), dados->faixa);
		else
			_tprintf_s(TEXT("Erro a criar/abrir key (%d)\n"), GetLastError());

		if (RegQueryValueExA(temp, keyValueNameV, NULL, NULL, (LPBYTE)&dados->velocity, &size) == ERROR_SUCCESS)
			_tprintf_s(TEXT("Velocity: %d\n"), dados->velocity);
		else
			_tprintf_s(TEXT("Erro a criar/abrir key (%d)\n"), GetLastError());
	}
	else { //Não existe regkey ainda
		dados->faixa = 10;
		dados->velocity = 1;
	}
	RegCloseKey(temp);
}