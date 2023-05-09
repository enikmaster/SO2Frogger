#include "servidor.h"
#define SHM_NAME TEXT("SHM_PC")
#define MUTEX_NAME TEXT("MUTEXP")
#define SEM_WRITE_NAME TEXT("SEM_WRITE")
#define SEM_READ_NAME TEXT("SEM_READ")

#define BUFFER_SIZE 10



BOOL initMemAndSync(ControlData* cData, Info* dados) {
	BOOL firstProcess = FALSE;

	cData->hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(SharedMem),
		SHM_NAME);

	if (cData->hMapFile == NULL)
	{
		_tprintf(TEXT("Error: CreateFileMapping (%d)\n"), GetLastError());
		return FALSE;
	}

	cData->sharedMem = (SharedMem*)MapViewOfFile(cData->hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(SharedMem));

	if (cData->sharedMem == NULL)
	{
		_tprintf(TEXT("Error: MapViewOfFile (%d)\n"), GetLastError());
		CloseHandle(cData->hMapFile);
		return FALSE;
	}

	if (firstProcess) {
		cData->sharedMem->p = 0;
		cData->sharedMem->c = 0;
		cData->sharedMem->wP = 0;
		cData->sharedMem->rP = 0;
	}

	cData->hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);

	if (cData->hMutex == NULL)
	{
		_tprintf(TEXT("Error: CreateMutex (%d)\n"), GetLastError());
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

	return TRUE;
}