#include "servidor.h"

int _tmain(int argc, TCHAR** argv) {
	if (argc) {

	};
#ifdef UNICODE
	DWORD x1, x2, x3;
	x1 = _setmode(_fileno(stdin), _O_WTEXT);
	x2 = _setmode(_fileno(stdout), _O_WTEXT);
	x3 = _setmode(_fileno(stderr), _O_WTEXT);
	if (x1 == -1 || x2 == -1 || x3 == -1)
		ExitProcess(-1);
#endif
	checkStart();
	checkArgs(argc - 1, argv);

	//_tprintf_s(TEXT("servidor")); // usar TEXT assegura que a string est� codificada devidamente
	//_tprintf_s(TEXT("Testando commit")); // usar TEXT assegura que a string est� codificada devidamente
	//_tprintf_s(TEXT("Agora vai"));
	while (1) {
		TCHAR hi[20];
		_fgetts(hi, 20, stdin);
		return 0;
	}
	return 0;
}