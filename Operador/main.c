#include "operador.h"
#include "..\Froggerino\froggerino.h"

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

DWORD WINAPI GetInput(LPVOID param) {
	infoextra* pdata = (infoextra*)param;
	
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Error: CreateFileMapping (%d)\n"), GetLastError());
		return FALSE;
	}
	pdata->controlingData.hMapFile = hMapFile;
	pdata->controlingData.sharedMem = (SharedMem*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (pdata->controlingData.sharedMem == NULL) {
		_tprintf(TEXT("Error: Alocação de memória deu erro\n"));
	}
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENTSHAREDMEM);
	if (hEvent == NULL) {
		ExitProcess(1);
	}

	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEXSHAREDMEM);
	pdata->controlingData.hEvent = hEvent;
	pdata->controlingData.hMutex = hMutex;
	TCHAR localBG[10][20];
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD pos;
	pos.X = 0;
	pos.Y = 5;	

	do {
		WaitForSingleObject(pdata->controlingData.hEvent, INFINITE);
		WaitForSingleObject(pdata->controlingData.hMutex, INFINITE);
		CopyMemory(&localBG, pdata->controlingData.sharedMem->gameShared, sizeof(pdata->controlingData.sharedMem->gameShared));
		ReleaseMutex(pdata->controlingData.hMutex);
		ResetEvent(pdata->controlingData.hEvent);
		WaitForSingleObject(pdata->hMutex, INFINITE);
		GetConsoleScreenBufferInfo(pdata->hStdout, &csbi);
		SetConsoleCursorPosition(pdata->hStdout, pos);
		_tprintf_s(TEXT("\n-    -    -    -    -    -    -    -    -    -    -    -    - \n"));
		showBG(localBG);
		SetConsoleCursorPosition(pdata->hStdout, csbi.dwCursorPosition);
		ReleaseMutex(pdata->hMutex);
		Sleep(500);
	} while (1);
	
	CloseHandle(pdata->controlingData.hMapFile);
	ExitThread(1);
}

// tentativa n8...
Comando checkInput(TCHAR input[256], infoextra* info) {
	TCHAR* token;
	TCHAR* next_token;
	int count = 0;
	int num = 0;
	TCHAR palavra[256] = TEXT("");
	TCHAR reversed[256] = TEXT("");
	BOOL need_reversing = FALSE;
	//TCHAR new_input[256] = TEXT("");
	//_tcscpy_s(new_input, sizeof(new_input)/sizeof(TCHAR), input);

	// Verifica se a string tem duas palavras separadas por espaço
	token = _tcstok_s(input, TEXT(" "), &next_token); // isto altera a string original - no no
	while (token != NULL) {
		count++;
		if (count == 1 && _stscanf_s(token, TEXT("%d"), &num) != 1) {
			// se entra aqui, o primeiro arg não é num
			_tcscpy_s(palavra, sizeof(palavra) / sizeof(TCHAR), token);
		}
		else if (count == 2 && _stscanf_s(token, TEXT("%d"), &num) != 1) {
			// se entra aqui, o segundo arg não é num
			need_reversing = TRUE;
			_tcscpy_s(palavra, sizeof(palavra)/sizeof(TCHAR), token);
		}
		token = _tcstok_s(NULL, TEXT(" "), &next_token);
	}
	// Verifica se a primeira palavra é um número entre 1 e 8
	if (num < 1 || num > 8) {
		return CMD_ERRO;
	}
	/*if (need_reversing) {
		// Inverte a ordem das palavras se necessário
		//_stprintf_s(reversed, sizeof(reversed), TEXT("%s %d"), palavra, num);
		if (count == 2 && _tcscmp(reversed, input) != 0) {
			_tcscpy_s(input, sizeof(input), reversed);
		}
	}*/
	// Verifica se a palavra é uma das opções válidas
	if (_tcscmp(palavra, TEXT("PARAR")) == 0) {
		return CMD_PARAR;
	}
	else if (_tcscmp(palavra, TEXT("ADICIONAR")) == 0) {
		return CMD_ADICIONAR;
	}
	else if (_tcscmp(palavra, TEXT("INVERTER")) == 0) {
		return CMD_INVERTER;
	}
	else {
		return CMD_ERRO;
	}
}

// tentativa n1
Comando _checkInput(TCHAR msg[256], infoextra* info) {
	TCHAR input[256];
	// Fazer uma cópia do original
	lstrcpyn(input, msg, sizeof(input) / sizeof(TCHAR));
	// Converter input para maiúsculas
	_tcsupr_s(input, sizeof(input));
	// Usar tokens para contar palavras
	TCHAR* token;
	TCHAR* next_token = NULL;
	TCHAR* last_token = NULL;
	TCHAR* first_token;
	TCHAR* reversed = TEXT("");
	int nTokens = 0;
	// Dividir a string em tokens
	token = _tcstok_s(input, TEXT(" \n\r\t"), &next_token);
	while (token != NULL) {
		++nTokens;
		last_token = token;
		token = _tcstok_s(NULL, TEXT(" \n\r\t"), &next_token);
	}
	// Verificar o número de argumentos
	if (nTokens != 2)
		return CMD_ERRO;
	else {
		TCHAR input_copy[256];
		// Copiar novamente a string original
		lstrcpyn(input_copy, msg, sizeof(input) / sizeof(TCHAR));
		// Separar o primeiro argumento
		first_token = _tcstok_s(input_copy, TEXT(" \n\r\t"), &next_token);
		// Verificar formato "int comando"
		if (_ttoi(first_token) >= 1 && _ttoi(first_token) <= 8) {
			if (_tcsstr(_tcsupr_s(last_token, sizeof(last_token)), TEXT("PARAR")) != NULL) {
				lstrcat(reversed, last_token);
				lstrcat(reversed, TEXT(" "));
				lstrcat(reversed, first_token);
				lstrcpyn(msg, reversed, sizeof(msg) / sizeof(TCHAR));
				return CMD_PARAR;
			}
			else if (_tcsstr(_tcsupr_s(last_token, sizeof(last_token)), TEXT("INVERTER")) != NULL) {
				lstrcat(reversed, last_token);
				lstrcat(reversed, TEXT(" "));
				lstrcat(reversed, first_token);
				lstrcpyn(msg, reversed, sizeof(msg) / sizeof(TCHAR));
				return CMD_INVERTER;
			}
			else if (_tcsstr(_tcsupr_s(last_token, sizeof(last_token)), TEXT("ADICIONAR")) != NULL) {
				lstrcat(reversed, last_token);
				lstrcat(reversed, TEXT(" "));
				lstrcat(reversed, first_token);
				lstrcpyn(msg, reversed, sizeof(msg) / sizeof(TCHAR));
				return CMD_ADICIONAR;
			}
		} else { // Verificar o formato "comando int"
			_tcsupr_s(first_token, sizeof(first_token));
			int num = _ttoi(last_token);
			if (_tcsstr(first_token, TEXT("PARAR")) != NULL)
				if (num >= 1 && num <= 8) // alterar o valor 8 que está hard coded como o noob que sou
					return CMD_PARAR;
			else if (_tcsstr(first_token, TEXT("INVERTER")) != NULL)
				if (num >= 1 && num <= 8) // alterar o valor 8 que está hard coded como o noob que sou
					return CMD_INVERTER;
			else if (_tcsstr(first_token, TEXT("ADICIONAR")) != NULL)
				if (num >= 1 && num <= 8) // alterar o valor 8 que está hard coded como o noob que sou
					return CMD_ADICIONAR;
		}
		
	}
	return CMD_ERRO;
}


int _tmain(int argc, TCHAR** argv) {
#ifdef UNICODE
	DWORD x1, x2, x3;
	x1 = _setmode(_fileno(stdin), _O_WTEXT);
	x2 = _setmode(_fileno(stdout), _O_WTEXT);
	x3 = _setmode(_fileno(stderr), _O_WTEXT);
	if (x1 == -1 || x2 == -1 || x3 == -1)
		ExitProcess(-1);
#endif
	COORD pos;
	DWORD res;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	pos.X = 0;
	pos.Y = 0;
	BOOL succeed = FillConsoleOutputCharacter(hStdout, _T(' '), 80 * 26, pos, &res);
	if (!succeed) {
		_tprintf_s(TEXT("Error"));
		ExitProcess(-1);
	}
	_tprintf_s(TEXT("Operador")); // usar TEXT assegura que a string está codificada devidamente

	
	HANDLE MutexShared = CreateMutex(NULL, FALSE, TEXT("MUTEXLOCAL"));

	
	infoextra extra;

	extra.hMutex = MutexShared;
	extra.hStdout = hStdout;
	HANDLE hThread = CreateThread(NULL, 0, GetInput, &extra, 0, NULL);
	extra.controlingData.hReadSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, SEM_READ_NAME);
	extra.controlingData.hWriteSem = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, SEM_WRITE_NAME);
	int count = 0;
	TCHAR msg[256];
	while (1) {
		DWORD written;
		CONSOLE_SCREEN_BUFFER_INFO info;

		pos.X = 0;
		pos.Y = 1;
		SetConsoleCursorPosition(hStdout, pos);
		WaitForSingleObject(extra.hMutex, INFINITE);
		_tprintf_s(TEXT("Comando a enviar: "));
		ReleaseMutex(extra.hMutex);
		pos.X = 18;
		pos.Y = 1;
		SetConsoleCursorPosition(hStdout, pos);
		_fgetts(msg, sizeof(msg) / sizeof(TCHAR), stdin);
		
		// verificações começam aqui
		Comando comando = checkInput(&msg, &extra);
		switch (comando) {
		case CMD_PARAR:
			pos.X = 0;
			pos.Y = 2;
			_tprintf_s(TEXT("Comando PARAR "));
			break;
		case CMD_INVERTER:
			pos.X = 0;
			pos.Y = 2;
			_tprintf_s(TEXT("Comando INVERTER "));
			break;
		case CMD_ADICIONAR:
			pos.X = 0;
			pos.Y = 2;
			_tprintf_s(TEXT("Comando ADICIONAR "));
			break;
		default:
			pos.X = 0;
			pos.Y = 2;
			_tprintf_s(TEXT("Comando inválido "));
			break;
		}

		WaitForSingleObject(extra.hMutex, INFINITE);
		if (count > 0) {
			pos.X = 0;
			pos.Y = 2;
			SetConsoleCursorPosition(hStdout, pos);
			FillConsoleOutputCharacter(hStdout, ' ', 50, pos, &written);
		}
		_tprintf_s(TEXT("Você enviou: %s"), msg);
		WaitForSingleObject(extra.controlingData.hWriteSem, INFINITE);
		WaitForSingleObject(extra.controlingData.hMutex, INFINITE);
		CopyMemory(&extra.controlingData.sharedMem->buffer[extra.controlingData.sharedMem->wP++], &msg, sizeof(extra.controlingData.sharedMem->buffer));
		if (extra.controlingData.sharedMem->wP == BUFFER_SIZE)
			extra.controlingData.sharedMem->wP = 0;
		ReleaseMutex(extra.controlingData.hMutex);
		ReleaseSemaphore(extra.controlingData.hReadSem,1,NULL);
		//enviar isto para bufferCircular msg
		count++;
		pos.Y = 1;
		COORD start = { pos.X, pos.Y };
		GetConsoleScreenBufferInfo(hStdout, &info);
		DWORD numChars = info.dwSize.X - pos.X;
		FillConsoleOutputCharacter(hStdout, TEXT(' '), numChars, start, &written);
		ReleaseMutex(extra.hMutex);
	}
	CloseHandle(extra.hMutex);
	return 0;
}
