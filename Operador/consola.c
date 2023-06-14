#include "operador.h"


void showBG(TCHAR localBG[10][20], DWORD TamFaixa) {
	for (int i = 0; i < TamFaixa + 2; i++) {
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

Comando checkInput(TCHAR* msg, infoextra* info, DWORD* AltFaixa, DWORD maxFaixas) {
	TCHAR palavra[TAMWORD] = { 0 };
	DWORD count = 0;
	TCHAR firstarg[TAMWORD] = { 0 };
	TCHAR secondArgStr[TAMARGSFAIXA] = { 0 };
	DWORD secondArg = 0;
	int first_arg_index = 0;
	int second_arg_index = 0;

	for (int i = 0; msg[i] != TEXT('\n'); i++) {
		if (msg[i] == TEXT(' '))
			++count;
		else {
			if (count == 0) {
				if (first_arg_index < sizeof(firstarg) - 1)
					firstarg[first_arg_index++] = msg[i];
				else
					return CMD_ERRO;
			}
			else if (count == 1) {
				if (second_arg_index < sizeof(secondArgStr) / sizeof(secondArgStr[0]) - 1)
					if (iswdigit(msg[i]))
						secondArgStr[second_arg_index++] = msg[i];
					else
						return CMD_ERRO;
				else
					return CMD_ERRO;
			}
		}
		if (count > 2)
			return CMD_ERRO;
	}
	secondArgStr[second_arg_index] = TEXT('\0');
	secondArg = _tstoi(secondArgStr);
	firstarg[first_arg_index] = TEXT('\0');
	if ((secondArg < 2 || secondArg > maxFaixas - 1) && count >= 1) {
		return CMD_ERRO;
	}
	*AltFaixa = secondArg;
	_tcsupr_s(firstarg, sizeof(firstarg) / sizeof(firstarg[0]));
	if (wcscmp(firstarg, TEXT("PARAR")) == 0) {
		return CMD_PARAR;
	}
	else if (wcscmp(firstarg, TEXT("ADICIONAR")) == 0) {
		if (secondArg < 2 || secondArg > maxFaixas - 1)
			return CMD_ERRO;
		return CMD_ADICIONAR;
	}
	else if (wcscmp(firstarg, TEXT("INVERTER")) == 0) {
		return CMD_INVERTER;
	}
	else if (wcscmp(firstarg, TEXT("RETOMAR")) == 0) {
		return CMD_RETOMAR;
	}
	else if (wcscmp(firstarg, TEXT("SAIR")) == 0) {
		return CMD_SAIR;
	}
	else if (wcscmp(firstarg, TEXT("PAUSAR")) == 0) {
		return CMD_PAUSEALL;
	}
	return CMD_ERRO;
}
