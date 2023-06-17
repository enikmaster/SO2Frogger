//#pragma comment( lib, "Msimg32" )
#include <windows.h>
#include <tchar.h>
//#include <math.h>
//#include <stdio.h>
//#include <fcntl.h>
//#include <io.h>
#include <windowsx.h>


//#define PIPE_NAME TEXT("\\\\.\\pipe\\SapoPipe")
#define PIPE_NAME TEXT("\\\\.\\pipe\\canal")
#define MAX_MESSAGE_SIZE 1024
#define IDM_SINGLE_PLAYER 1001
#define IDM_MULTI_PLAYER 1002
#define IDM_ABOUT 1003
#define IDM_EXIT 1004
#define BUFSIZE 4096

#define SAPO TEXT('S')
#define CARRO TEXT('C')
#define OBSTACULO TEXT('O')
#define DESLOCAMENTO 34

// estrutura com os dados de um objeto do tabuleiro
typedef struct OBJETO {
	int posicaoX;
	int posicaoY;
	TCHAR tipo;
};
// estrutura de dados locais do jogo
typedef struct LOCAL {
	int nivel;
	int vidas;
	int tempo;
	int pontuacao;
	int numeroFaixas;
	DWORD nBytesRead;
	TCHAR mensagem[BUFSIZE];
	TCHAR mensagemaEnviar[BUFSIZE];
	OBJETO objetos[200];
	HANDLE hPipe;
	HWND hWnd;
	CRITICAL_SECTION cs;
	HANDLE hEventoEnviaMensagem;
};

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);

void ParseMessage(LOCAL* origenate) {

	

	if (wcslen(origenate->mensagem) == 0) {
		// mensagem vazia, encerrou o servidor
		return;
	}
	if (wcslen(origenate->mensagem) < 30) {
		// mensagem com informa��es complementares ao jogo (terminou, ganhou, encerrou)
	}
	else {
		// extrair a informa��o da mensagem com strtok
		// nivel vidas tempo pontua��o wwwwwwwwwwwwwwwwwwwwc c c c c c c c c c ...\0
		// copia a mensagem para a estrutura LOCAL
		TCHAR* next_token = NULL;
		//_tcscpy_s(local.mensagem, sizeof(local.mensagem), mensagem);
		// extrai o nivel
		TCHAR* token = _tcstok_s((TCHAR*)origenate->mensagem, TEXT("\0"), &next_token);
		//local.nivel = _ttoi(token);
		// extrai as vidas
		//token = _tcstok_s(NULL, TEXT(" "), &next_token);
		//local.vidas = _ttoi(token);
		// extrai o tempo
		//token = _tcstok_s(NULL, TEXT(" "), &next_token);
		//local.tempo = _ttoi(token);
		// extrai a pontua��o
		//token = _tcstok_s(NULL, TEXT(" "), &next_token);
		//local.pontuacao = _ttoi(token);

		//token = _tcstok_s(NULL, TEXT("\0"), &next_token);
		origenate->numeroFaixas = 10;
		// criar os objetos da estrutura LOCAL a partir da mensagem recebida
		for (int y = 0; y < origenate->numeroFaixas; ++y) {
			for (int x = 0; x < 20; ++x) {
				origenate->objetos[20 * y + x].posicaoX = x;
				origenate->objetos[20 * y + x].posicaoY = y;
				origenate->objetos[20 * y + x].tipo = token[20 * y + x];
			}
		}
	}
	return;
}
// fun��o para ler a mensagens do named pipe
DWORD WINAPI lerMessages(LPVOID param) {
	LOCAL* origem = (LOCAL*)param;
	HANDLE hPipe = origem->hPipe;
	BOOL res;
	LOCAL x;
	OVERLAPPED ov;

	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hEvent == NULL) {
		_tprintf_s(TEXT("Erro na cria��o do evento\n"));
		ExitProcess(-1);
	}
	do {
		ZeroMemory(&x, sizeof(x));
		ZeroMemory(&ov, sizeof(ov));
		ov.hEvent = hEvent;
		res = ReadFile(hPipe, x.mensagem, BUFSIZE, &x.nBytesRead, &ov);
		if (res)
			;
		else if (GetLastError() == ERROR_IO_PENDING) {
			//vai esperar que o evento seja sinalizado em overlapped
			WaitForSingleObject(hEvent, INFINITE);
			GetOverlappedResult(hPipe, &ov, &x.nBytesRead, FALSE);
		}
		else
			break;
		if (x.nBytesRead > 0) {
			
			//x = ParseMessage(x.mensagem);
			EnterCriticalSection(&origem->cs);
			wcscpy_s(origem->mensagem, x.mensagem);
			origem->nBytesRead = x.nBytesRead;
			ParseMessage(origem);
			LeaveCriticalSection(&origem->cs);
			InvalidateRect(origem->hWnd, // handle da janela
				NULL, // retangulo a atualizar (NULL = toda a janela)
				TRUE); // for�a a atualiza��o da janela
		}


	} while (x.nBytesRead > 0);   // NPipe foi encerrado pela thread inicial...
	ExitThread(0);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	HANDLE hPipe;	// hPipe � o handler do pipe
	TCHAR szMessage[MAX_MESSAGE_SIZE];
	DWORD bytesRead;
	HWND hWnd;		// hWnd � o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg;		// MSG � uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp;	// WNDCLASSEX � uma estrutura cujos membros servem para definir as caracter�sticas da classe da janela

	hPipe = CreateFile(
		PIPE_NAME,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL);
	if (hPipe == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, TEXT("Erro na abertura do pipe"), TEXT("Erro"), MB_OK);
		ExitProcess(-1);
	}
	ConnectNamedPipe(hPipe, NULL);
	if (!WaitNamedPipe(PIPE_NAME, 100)) {
		MessageBox(NULL, TEXT("Erro na conex�o do pipe"), TEXT("Erro"), MB_OK);
		ExitProcess(-1);
	}
	
	
	LOCAL local;
	local.hPipe = hPipe;
	local.cs;

	InitializeCriticalSection(&local.cs);
	local.hEventoEnviaMensagem = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (local.hEventoEnviaMensagem == NULL) {
		_tprintf_s(TEXT("Erro na cria��o do evento (%d)\n"), GetLastError());
		ExitProcess(-1);
	}
	
	// ============================================================================
	// 1. Defini��o das caracter�sticas da janela "wcApp" 
	//    (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
	// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX);      // Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;		        // Inst�ncia da janela actualmente exibida ("hInst" � par�metro de WinMain e vem inicializada da�)
	wcApp.lpszClassName = TEXT("Sapo");       // Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos;       // Endere�o da fun��o de processamento da janela ("TrataEventos" foi declarada no in�cio e encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;  // Estilo da janela: Fazer o redraw se for modificada horizontal ou verticalmente

	wcApp.hIcon = LoadIcon(NULL, IDI_WARNING);   // "hIcon" = handler do �con normal
	// "NULL" = Icon definido no Windows
	// "IDI_AP..." �cone "aplica��o"
	wcApp.hIconSm = LoadIcon(NULL, IDI_SHIELD);  // "hIconSm" = handler do �con pequeno
	// "NULL" = Icon definido no Windows
	// "IDI_INF..." �con de informa��o
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW); // "hCursor" = handler do cursor (rato) 
	// "NULL" = Forma definida no Windows
	// "IDC_ARROW" Aspecto "seta" 
	wcApp.lpszMenuName = NULL;	// Classe do menu que a janela pode ter (NULL = n�o tem menu)
	wcApp.cbClsExtra = 0;		// Livre, para uso particular
	wcApp.cbWndExtra = 0;		// Livre, para uso particular
	wcApp.hbrBackground = CreateSolidBrush(RGB(55, 55, 55)); // "hbrBackground" = handler para "brush" de pintura do background. Devolvido por "GetStockObject".

	// ============================================================================
	// 2. Registo da classe da janela
	// ============================================================================
	if (!RegisterClassEx(&wcApp))
		return FALSE; // Regista a classe da janela. Se n�o for bem sucedido, termina o programa

	// ============================================================================
	// 3. Cria��o da janela
	// ============================================================================
	hWnd = CreateWindow(
		TEXT("Sapo"),				// Nome da janela
		TEXT("Sapo"),			// T�tulo da janela
		WS_OVERLAPPEDWINDOW,	// Estilo da janela
		250,					// Posi��o x default
		250,					// Posi��o y default
		800,					// Comprimento default
		500,					// Altura default
		(HWND)HWND_DESKTOP,		// Janela-pai
		(HMENU)NULL,			// Menu
		(HINSTANCE)hInst,		// Inst�ncia
		NULL);				// Dados de cria��o
	if (!hWnd)
		return FALSE; // Se n�o for bem sucedido, termina o programa
	local.hWnd = hWnd;
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)&local);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)lerMessages, (LPVOID)&local, 0, NULL); //Thread recebe MARAVILHA
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)enviaMensagens, (LPVOID)&local, 0, NULL); //Thread Mensagem	string direita ou esquerda ou cima ou baixo
	// ============================================================================
	// 4. Mostra a janela
	// ============================================================================
	ShowWindow(hWnd, nCmdShow); // Mostra a janela
	UpdateWindow(hWnd);			// Desenha a janela

	// ============================================================================
	// 5. Processamento de mensagens
	// ============================================================================
	while (GetMessage( &lpMsg, NULL, 0, 0))	{ // Processa as mensagens
		TranslateMessage(&lpMsg);	// Pr�-processamento das mensagens
		DispatchMessage(&lpMsg);	// Envia as mensagens
	}
	// fecha o pipe
	CloseHandle(hPipe);
	ExitThread(0);

	return (int)lpMsg.wParam;		// Devolve o par�metro "wParam" da estrutura "lpMsg"
}

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	LOCAL* pLocal = NULL;

	// Retrieve the address of the 'local' variable associated with the window handle
	pLocal = (LOCAL*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	
	// TODO: fazer as vidas, pontos, tempo e nivel do jogo
	// o tempo � uma barra de progresso que desenha 1 elemento de cada vez

	static HDC bmpDCBackground = NULL;
	static HDC bmpDCSapo = NULL;
	static HDC bmpDCCarro = NULL;
	static HDC bmpDCObstaculo = NULL;

	static HBITMAP hBmpBackground = NULL;
	static HBITMAP hBmpSapo = NULL;
	static HBITMAP hBmpCarro = NULL;
	static HBITMAP hBmpObstaculo = NULL;

	static BITMAP bmpBackground;
	static BITMAP bmpSapo;
	static BITMAP bmpCarro;
	static BITMAP bmpObstaculo;

	static int xBackground, yBackground;
	static int xSapo, ySapo;
	static int xCarro, yCarro;
	static int xObstaculo, yObstaculo;
	
	static HANDLE hMutex;
	switch (messg) {
	case WM_CREATE:
	{// criar um menu com op��es de single player e multiplayer
		HMENU hMenu, hSubMenu;
		hMenu = CreateMenu();
		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, IDM_SINGLE_PLAYER, TEXT("Single Player"));
		AppendMenu(hSubMenu, MF_STRING, IDM_MULTI_PLAYER, TEXT("Multiplayer"));
		AppendMenu(hSubMenu, MF_STRING, IDM_EXIT, TEXT("Exit"));
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("Jogo"));
		SetMenu(hWnd, hMenu);
		// criar um menu About que abra uma janela com informa��o sobre o jogo
		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, IDM_ABOUT, TEXT("About"));
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("About"));
		SetMenu(hWnd, hMenu);
	}

	// fazer load dos diferentes bitmaps
	hBmpBackground = (HBITMAP)LoadImage(NULL, TEXT("background.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpBackground == NULL) {
		MessageBox(hWnd, TEXT("N�o foi poss�vel carregar o bitmap do background!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpObstaculo = (HBITMAP)LoadImage(NULL, TEXT("obstaculo.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpObstaculo == NULL) {
		MessageBox(hWnd, TEXT("N�o foi poss�vel carregar o bitmap do obstaculo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpCarro = (HBITMAP)LoadImage(NULL, TEXT("carro.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpCarro == NULL) {
		MessageBox(hWnd, TEXT("N�o foi poss�vel carregar o bitmap do carro!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpSapo = (HBITMAP)LoadImage(NULL, TEXT("sapo.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpSapo == NULL) {
		MessageBox(hWnd, TEXT("N�o foi poss�vel carregar o bitmap do sapo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	// obter os tamanhos dos diferentes bitmaps
	GetObject(hBmpBackground, sizeof(BITMAP), &bmpBackground);
	GetObject(hBmpObstaculo, sizeof(BITMAP), &bmpObstaculo);
	GetObject(hBmpCarro, sizeof(BITMAP), &bmpCarro);
	GetObject(hBmpSapo, sizeof(BITMAP), &bmpSapo);
	// criar o mutex
	hMutex = CreateMutex(NULL, FALSE, NULL);
	// ########################################### Background ###########################################
	// criar o DC do bitmap do background
	hdc = GetDC(hWnd);
	// criar o DC do bitmap do background
	bmpDCBackground = CreateCompatibleDC(hdc);
	// selecionar o bitmap do background no DC
	SelectObject(bmpDCBackground, hBmpBackground);
	// obter o tamanho da janela
	GetClientRect(hWnd, &rect);
	// calcular a posi��o do bitmap do background
	xBackground = 0;
	yBackground = rect.bottom - bmpBackground.bmHeight;
	// ########################################### Obstaculo ###########################################
	// criar o DC do bitmap do obstaculo
	bmpDCObstaculo = CreateCompatibleDC(hdc);
	// selecionar o bitmap do obstaculo no DC
	SelectObject(bmpDCObstaculo, hBmpObstaculo);
	// obter o tamanho da janela
	GetClientRect(hWnd, &rect);
	// calcular a posi��o do bitmap do obstaculo no fundo da janela
	xObstaculo = 0;
	yObstaculo = 4 + bmpObstaculo.bmHeight;
	// ########################################### Carro ###########################################
	// criar o DC do bitmap do carro
	bmpDCCarro = CreateCompatibleDC(hdc);
	// selecionar o bitmap do carro no DC
	SelectObject(bmpDCCarro, hBmpCarro);
	// obter o tamanho da janela
	GetClientRect(hWnd, &rect);
	// calcular a posi��o do bitmap do carro no fundo da janela
	xCarro = 0;
	yCarro = 50;
	// ########################################### Sapo ###########################################
	// criar o DC do bitmap do sapo
	bmpDCSapo = CreateCompatibleDC(hdc);
	// selecionar o bitmap do sapo no DC
	SelectObject(bmpDCSapo, hBmpSapo);
	// obter o tamanho da janela
	GetClientRect(hWnd, &rect);
	// calcular a posi��o do bitmap do sapo em rela��o ao background
	xSapo = xBackground + 4;
	ySapo = bmpBackground.bmHeight - bmpSapo.bmHeight - 2;
	// libertar o DC
	ReleaseDC(hWnd, hdc);

	break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		FillRect(hdc, &rect, CreateSolidBrush(RGB(0, 0, 0)));
		
		BitBlt(hdc, xBackground, yBackground, bmpBackground.bmWidth, bmpBackground.bmHeight, bmpDCBackground, 0, 0, SRCCOPY);
		if (pLocal != nullptr) {
			for (int i = 0; i < pLocal->numeroFaixas * 20; ++i) {
				switch (pLocal->objetos[i].tipo) {
				case CARRO:
					BitBlt(hdc, pLocal->objetos[i].posicaoX * DESLOCAMENTO, pLocal->objetos[i].posicaoY * DESLOCAMENTO, bmpCarro.bmWidth, bmpCarro.bmHeight, bmpDCCarro, 0, 0, SRCCOPY);
					break;
				case SAPO:
					BitBlt(hdc, pLocal->objetos[i].posicaoX * DESLOCAMENTO, pLocal->objetos[i].posicaoY * DESLOCAMENTO, bmpSapo.bmWidth, bmpSapo.bmHeight, bmpDCSapo, 0, 0, SRCCOPY);
					break;
				case OBSTACULO:
					BitBlt(hdc, pLocal->objetos[i].posicaoX * DESLOCAMENTO, pLocal->objetos[i].posicaoY * DESLOCAMENTO, bmpObstaculo.bmWidth, bmpObstaculo.bmHeight, bmpDCObstaculo, 0, 0, SRCCOPY);
					break;
				default:
					break;
				}
			}
		}

		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		// obter o tamanho da janela
		GetClientRect(hWnd, &rect);
		// alterar a posi��o do bitmap do background
		xBackground = (rect.right - rect.left - bmpBackground.bmWidth) / 2;
		yBackground = (rect.bottom - rect.top - bmpBackground.bmHeight) / 2;
		// alterar a posi��o do bitmap do obstaculo em rela��o ao background
		xObstaculo = xBackground + 4;
		yObstaculo = yBackground + bmpObstaculo.bmHeight + 2;
		// alterar a posi��o do bitmap do carro em rela��o ao background
		xCarro = xBackground + 4;
		yCarro = yBackground + (bmpCarro.bmHeight + 4) * 3;
		// alterar a posi��o do bitmap do sapo em rela��o ao background
		xSapo = xBackground + 4;
		ySapo = yBackground + bmpBackground.bmHeight - bmpSapo.bmHeight;

		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			// caso a op��o "Single player" do menu "Novo jogo" seja selecionada
		case IDM_SINGLE_PLAYER:
			/*
			// ler a informa��o da struct comum com os dados do jogo
			//WaitForSingleObject(hMutex, INFINITE);
			// obter o tamanho da janela
			GetClientRect(hWnd, &rect);
			// alterar a posi��o do bitmap do background para o colocar numa posi��o inicial no centro da janela
			xBackground = (rect.right - rect.left - bmpBackground.bmWidth) / 2;
			yBackground = (rect.bottom - rect.top - bmpBackground.bmHeight) / 2;
			// por cada obstaculo que existir na struct comum alterar a posi��o do bitmap do obstaculo em rela��o ao background
			for (int i = 0; i < 20; i++) {
				xObstaculo = xBackground + 4;
				yObstaculo = yBackground + bmpObstaculo.bmHeight + 2;
				xObstaculo += (bmpObstaculo.bmWidth + 4) * i;
			}
			*/
			//SetEvento(local.hEvento);
			MessageBox(hWnd, TEXT("Single Player selecionado"), TEXT("Confirma��o"), MB_OK | MB_ICONERROR);
			break;
		case IDM_MULTI_PLAYER:
			MessageBox(hWnd, TEXT("Multi Player selecionado"), TEXT("Confirma��o"), MB_OK | MB_ICONERROR);
			break;
		case IDM_EXIT:
			if (MessageBox(hWnd, TEXT("Deseja mesmo sair?"), TEXT("Sair"), MB_YESNO | MB_ICONQUESTION) == IDYES)
				DestroyWindow(hWnd);
			break;
		case IDM_ABOUT:
			MessageBox(hWnd, TEXT("Trabalho elaborado por Louren�o McBride e Filipe Carvalho"), TEXT("Sobre"), MB_OK | MB_ICONINFORMATION);
			break;
		}
		break;
		
	case WM_CLOSE:
		if (MessageBox(hWnd, TEXT("Deseja mesmo sair?"), TEXT("Sair"), MB_YESNO | MB_ICONQUESTION) == IDYES)
			DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, messg, wParam, lParam);
	}
	return 0;
}



