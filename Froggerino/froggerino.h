// froggerino.h - cont�m as declara��es das fun��es partilhadas do frogger
#pragma once

#ifdef FROGGERINO_EXPORTS
#define FROGGERINO_API __declspec(dllexport)
#else
#define FROGGERINO_API __declspec(dllimport)
#endif

FROGGERINO_API int x;

FROGGERINO_API int primeiraFunction();
