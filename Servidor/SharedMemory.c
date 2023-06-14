#include "servidor.h"
#include "..\Froggerino\froggerino.h"

#define SHM_NAME TEXT("SHM_PC")
#define MUTEX_NAME TEXT("MUTEXP")
#define SEM_WRITE_NAME TEXT("SEM_WRITE")
#define SEM_READ_NAME TEXT("SEM_READ")

// coisas a partilhar:
// tabuleiro
// estrutura de Info (pontuação, nível e tempo)
// buffer circular


BOOL initMemAndSync(ControlData* cData, Info* dados,DWORD x) {
	//BOOL firstProcess = TRUE;

	// cria fileMapping
	cData->hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(SharedMem),
		SHM_NAME);

	if (cData->hMapFile == NULL) {
		_tprintf(TEXT("[ERRO] Erro a crair o FileMapping.\n"));
		CloseHandle(cData->hMapFile);
		return FALSE;
	}

	cData->sharedMem = (SharedMem*)MapViewOfFile(cData->hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(SharedMem));

	if (cData->sharedMem == NULL)
	{
		_tprintf(TEXT("[ERRO] Erro a criar o MapViewOfFile.\n"));
		CloseHandle(cData->hMapFile);
		return FALSE;
	}
	
	cData->sharedMem->p = 0;
	cData->sharedMem->c = 0;
	cData->sharedMem->wP = 0;
	cData->sharedMem->rP = 0;
	

	cData->hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
	if (cData->hMutex == NULL) {
		_tprintf(TEXT("[ERRO] Erro a criar o Mutex.\n"));
		UnmapViewOfFile(cData->sharedMem);
		CloseHandle(cData->hMapFile);
		CloseHandle(cData->hMutex);
		return FALSE;
	}

	cData->hWriteSem = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEM_WRITE_NAME);

	if (cData->hWriteSem == NULL)
	{
		_tprintf(TEXT("Error: CreateEvent (%d)\n"), GetLastError());
		UnmapViewOfFile(cData->sharedMem);
		CloseHandle(cData->hMapFile);
		CloseHandle(cData->hMutex);
		CloseHandle(cData->hWriteSem);
		return FALSE;
	}
	
	cData->hReadSem = CreateSemaphore(NULL, 0, 10, SEM_READ_NAME);

	if (cData->hReadSem == NULL)
	{
		_tprintf(TEXT("Error: CreateEvent (%d)\n"), GetLastError());
		UnmapViewOfFile(cData->sharedMem);
		CloseHandle(cData->hMapFile);
		CloseHandle(cData->hMutex);
		CloseHandle(cData->hWriteSem);
		CloseHandle(cData->hReadSem);
		return FALSE;
	}
	for (int i = 0; i < x+2; i++)
		for (int j = 0; j < 20; j++)
			cData->sharedMem->gameShared[i][j] = dados->arrayGame[i][j];
	cData->sharedMem->faixaMax = x;
	//_tprintf_s(TEXT("Entrou"));
	return TRUE;
}