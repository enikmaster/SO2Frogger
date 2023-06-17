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
#define IDM_COR_LIGHT 1005
#define IDM_COR_DARK 1006
#define BUFSIZE 4096

#define SAPO TEXT('S')
#define CARRO TEXT('C')
#define OBSTACULO TEXT('O')
#define DESLOCAMENTO 36

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
	DWORD jumpSucesso;
};

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);

void ParseMessage(LOCAL* origenate) {
	if (wcslen(origenate->mensagem) == 0) {
		// mensagem vazia, encerrou o servidor
		return;
	}
	if (wcslen(origenate->mensagem) < 30) {
		// mensagem com informações complementares ao jogo (terminou, ganhou, encerrou)
	}
	else {
		// extrair a informação da mensagem com strtok
		// nivel vidas tempo pontuação wwwwwwwwwwwwwwwwwwwwc c c c c c c c c c ...\0
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
		// extrai a pontuação
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
// função para ler a mensagens do named pipe
DWORD WINAPI lerMessages(LPVOID param) {
	LOCAL* origem = (LOCAL*)param;
	HANDLE hPipe = origem->hPipe;
	BOOL res;
	LOCAL x;
	OVERLAPPED ov;

	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hEvent == NULL) {
		_tprintf_s(TEXT("Erro na criação do evento\n"));
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
				TRUE); // força a atualização da janela
		}


	} while (x.nBytesRead > 0);   // NPipe foi encerrado pela thread inicial...
	ExitThread(0);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	HANDLE hPipe;	// hPipe é o handler do pipe
	TCHAR szMessage[MAX_MESSAGE_SIZE];
	DWORD bytesRead;
	HWND hWnd;		// hWnd é o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg;		// MSG é uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp;	// WNDCLASSEX é uma estrutura cujos membros servem para definir as características da classe da janela

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
		MessageBox(NULL, TEXT("Erro na conexão do pipe"), TEXT("Erro"), MB_OK);
		ExitProcess(-1);
	}
	
	
	LOCAL local;
	local.hPipe = hPipe;
	local.cs;

	InitializeCriticalSection(&local.cs);
	local.hEventoEnviaMensagem = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (local.hEventoEnviaMensagem == NULL) {
		_tprintf_s(TEXT("Erro na criação do evento (%d)\n"), GetLastError());
		ExitProcess(-1);
	}
	
	// ============================================================================
	// 1. Definição das características da janela "wcApp" 
	//    (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
	// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX);      // Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;		        // Instância da janela actualmente exibida ("hInst" é parâmetro de WinMain e vem inicializada daí)
	wcApp.lpszClassName = TEXT("Sapo");       // Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos;       // Endereço da função de processamento da janela ("TrataEventos" foi declarada no início e encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;  // Estilo da janela: Fazer o redraw se for modificada horizontal ou verticalmente

	wcApp.hIcon = LoadIcon(NULL, IDI_WARNING);   // "hIcon" = handler do ícon normal
	// "NULL" = Icon definido no Windows
	// "IDI_AP..." Ícone "aplicação"
	wcApp.hIconSm = LoadIcon(NULL, IDI_SHIELD);  // "hIconSm" = handler do ícon pequeno
	// "NULL" = Icon definido no Windows
	// "IDI_INF..." Ícon de informação
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW); // "hCursor" = handler do cursor (rato) 
	// "NULL" = Forma definida no Windows
	// "IDC_ARROW" Aspecto "seta" 
	wcApp.lpszMenuName = NULL;	// Classe do menu que a janela pode ter (NULL = não tem menu)
	wcApp.cbClsExtra = 0;		// Livre, para uso particular
	wcApp.cbWndExtra = 0;		// Livre, para uso particular
	wcApp.hbrBackground = CreateSolidBrush(RGB(55, 55, 55)); // "hbrBackground" = handler para "brush" de pintura do background. Devolvido por "GetStockObject".

	// ============================================================================
	// 2. Registo da classe da janela
	// ============================================================================
	if (!RegisterClassEx(&wcApp))
		return FALSE; // Regista a classe da janela. Se não for bem sucedido, termina o programa

	// ============================================================================
	// 3. Criação da janela
	// ============================================================================
	hWnd = CreateWindow(
		TEXT("Sapo"),				// Nome da janela
		TEXT("Sapo"),			// Título da janela
		WS_OVERLAPPEDWINDOW,	// Estilo da janela
		250,					// Posição x default
		250,					// Posição y default
		800,					// Comprimento default
		500,					// Altura default
		(HWND)HWND_DESKTOP,		// Janela-pai
		(HMENU)NULL,			// Menu
		(HINSTANCE)hInst,		// Instância
		NULL);				// Dados de criação
	if (!hWnd)
		return FALSE; // Se não for bem sucedido, termina o programa
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
		TranslateMessage(&lpMsg);	// Pré-processamento das mensagens
		DispatchMessage(&lpMsg);	// Envia as mensagens
	}
	// fecha o pipe
	CloseHandle(hPipe);
	ExitThread(0);

	return (int)lpMsg.wParam;		// Devolve o parâmetro "wParam" da estrutura "lpMsg"
}

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	LOCAL* pLocal = NULL;

	// Retrieve the address of the 'local' variable associated with the window handle
	pLocal = (LOCAL*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	static int theme = 1;
	
	// TODO: fazer as vidas, pontos, tempo e nivel do jogo
	// o tempo é uma barra de progresso que desenha 1 elemento de cada vez

	static HDC bmpDCBackground = NULL;
	static HDC bmpDCSapo = NULL;
	static HDC bmpDCCarro = NULL;
	static HDC bmpDCObstaculo = NULL;
	static HDC bmpDCBannerTempo = NULL;
	static HDC bmpDCBarraTempo = NULL;
	static HDC bmpDCVidas = NULL;
	static HDC bmpDCTitulo = NULL;

	static HBITMAP hBmpBackground = NULL;
	static HBITMAP hBmpSapo = NULL;
	static HBITMAP hBmpCarro = NULL;
	static HBITMAP hBmpObstaculo = NULL;
	static HBITMAP hBmpBannerTempo = NULL;
	static HBITMAP hBmpBarraTempo = NULL;
	static HBITMAP hBmpVidas = NULL;
	static HBITMAP hBmpTitulo = NULL;

	static BITMAP bmpBackground;
	static BITMAP bmpSapo;
	static BITMAP bmpCarro;
	static BITMAP bmpObstaculo;
	static BITMAP bmpBannerTempo;
	static BITMAP bmpBarraTempo;
	static BITMAP bmpVidas;
	static BITMAP bmpTitulo;

	static int xBackground, yBackground;
	static int xSapo, ySapo;
	static int xCarro, yCarro;
	static int xObstaculo, yObstaculo;
	static int xBannerTempo, yBannerTempo;
	static int xBarraTempo, yBarraTempo;
	static int xVidas, yVidas;
	static int xTitulo, yTitulo;
	
	static HANDLE hMutex;
	switch (messg) {
	case WM_CREATE:
	{// criar um menu com opções de single player e multiplayer
		HMENU hMenu, hSubMenu;
		hMenu = CreateMenu();
		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, IDM_SINGLE_PLAYER, TEXT("Single Player"));
		AppendMenu(hSubMenu, MF_STRING, IDM_MULTI_PLAYER, TEXT("Multiplayer"));
		AppendMenu(hSubMenu, MF_STRING, IDM_EXIT, TEXT("Exit"));
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("Jogo"));
		SetMenu(hWnd, hMenu);
		// criar um menu Opções com opções de cor
		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, IDM_COR_LIGHT, TEXT("Escuro"));
		AppendMenu(hSubMenu, MF_STRING, IDM_COR_DARK, TEXT("Claro"));
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("Opções"));
		SetMenu(hWnd, hMenu);
		// criar um menu About que abra uma janela com informação sobre o jogo
		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, IDM_ABOUT, TEXT("About"));
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("About"));
		SetMenu(hWnd, hMenu);
	}

	// fazer load dos diferentes bitmaps
	hBmpBackground = (HBITMAP)LoadImage(NULL, TEXT("background.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpBackground == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do background!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpObstaculo = (HBITMAP)LoadImage(NULL, TEXT("obstaculo.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpObstaculo == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do obstaculo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpCarro = (HBITMAP)LoadImage(NULL, TEXT("carro.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpCarro == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do carro!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpSapo = (HBITMAP)LoadImage(NULL, TEXT("sapo.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpSapo == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do sapo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpBannerTempo = (HBITMAP)LoadImage(NULL, TEXT("tempo.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpBannerTempo == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do banner do tempo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpBarraTempo = (HBITMAP)LoadImage(NULL, TEXT("timer.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpBarraTempo == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap da barra do tempo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpVidas = (HBITMAP)LoadImage(NULL, TEXT("vida.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpVidas == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap das vidas!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpTitulo = (HBITMAP)LoadImage(NULL, TEXT("title.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpTitulo == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do titulo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	// obter os tamanhos dos diferentes bitmaps
	GetObject(hBmpBackground, sizeof(BITMAP), &bmpBackground);
	GetObject(hBmpObstaculo, sizeof(BITMAP), &bmpObstaculo);
	GetObject(hBmpCarro, sizeof(BITMAP), &bmpCarro);
	GetObject(hBmpSapo, sizeof(BITMAP), &bmpSapo);
	GetObject(hBmpBannerTempo, sizeof(BITMAP), &bmpBannerTempo);
	GetObject(hBmpBarraTempo, sizeof(BITMAP), &bmpBarraTempo);
	GetObject(hBmpVidas, sizeof(BITMAP), &bmpVidas);
	GetObject(hBmpTitulo, sizeof(BITMAP), &bmpTitulo);
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
	// calcular a posição do bitmap do background
	xBackground = 0;
	yBackground = rect.bottom - bmpBackground.bmHeight;
	// ########################################### Obstaculo ###########################################
	// criar o DC do bitmap do obstaculo
	bmpDCObstaculo = CreateCompatibleDC(hdc);
	// selecionar o bitmap do obstaculo no DC
	SelectObject(bmpDCObstaculo, hBmpObstaculo);
	// obter o tamanho da janela
	GetClientRect(hWnd, &rect);
	// calcular a posição do bitmap do obstaculo no fundo da janela
	xObstaculo = 0;
	yObstaculo = 4 + bmpObstaculo.bmHeight;
	// ########################################### Carro ###########################################
	// criar o DC do bitmap do carro
	bmpDCCarro = CreateCompatibleDC(hdc);
	// selecionar o bitmap do carro no DC
	SelectObject(bmpDCCarro, hBmpCarro);
	// obter o tamanho da janela
	GetClientRect(hWnd, &rect);
	// calcular a posição do bitmap do carro no fundo da janela
	xCarro = 0;
	yCarro = 50;
	// ########################################### Sapo ###########################################
	// criar o DC do bitmap do sapo
	bmpDCSapo = CreateCompatibleDC(hdc);
	// selecionar o bitmap do sapo no DC
	SelectObject(bmpDCSapo, hBmpSapo);
	// obter o tamanho da janela
	GetClientRect(hWnd, &rect);
	// calcular a posição do bitmap do sapo em relação ao background
	xSapo = xBackground + 4;
	ySapo = bmpBackground.bmHeight - bmpSapo.bmHeight - 2;
	// ########################################### Banner Tempo ###########################################
	// criar o DC do bitmap do banner do tempo
	bmpDCBannerTempo = CreateCompatibleDC(hdc);
	// selecionar o bitmap do banner do tempo no DC
	SelectObject(bmpDCBannerTempo, hBmpBannerTempo);
	// obter o tamanho da janela
	GetClientRect(hWnd, &rect);
	// calcular a posição do bitmap do banner do tempo em relação ao background

	// TODO: alterar a posição do banner do tempo
	xBannerTempo = xBackground + bmpBackground.bmWidth - bmpBannerTempo.bmWidth;
	yBannerTempo = yBackground + bmpBackground.bmHeight;
	// ########################################### Barra Tempo ###########################################
	// criar o DC do bitmap da barra do tempo
	bmpDCBarraTempo = CreateCompatibleDC(hdc);
	// selecionar o bitmap da barra do tempo no DC
	SelectObject(bmpDCBarraTempo, hBmpBarraTempo);
	// obter o tamanho da janela
	GetClientRect(hWnd, &rect);
	// calcular a posição do bitmap da barra do tempo em relação ao banner do tempo
	xBarraTempo = xBannerTempo - 4;
	yBarraTempo = yBannerTempo;
	// ########################################### Vidas ###########################################
	// criar o DC do bitmap das vidas
	bmpDCVidas = CreateCompatibleDC(hdc);
	// selecionar o bitmap das vidas no DC
	SelectObject(bmpDCVidas, hBmpVidas);
	// obter o tamanho da janela
	GetClientRect(hWnd, &rect);
	// calcular a posição do bitmap das vidas em relação ao banner do tempo
	xVidas = xBackground;
	yVidas = yBackground + bmpBackground.bmHeight;
	// ########################################### Titulo ###########################################
	// criar o DC do bitmap do titulo
	bmpDCTitulo = CreateCompatibleDC(hdc);
	// selecionar o bitmap do titulo no DC
	SelectObject(bmpDCTitulo, hBmpTitulo);
	// obter o tamanho da janela
	GetClientRect(hWnd, &rect);
	// calcular a posição do bitmap do titulo em relação ao background
	xTitulo = xBackground;
	yTitulo = yBackground - bmpTitulo.bmHeight;
	// libertar o DC
	ReleaseDC(hWnd, hdc);

	break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		FillRect(hdc, &rect, CreateSolidBrush(RGB(0, 0, 0)));

		switch (theme) {
		case 2: {
			BitBlt(hdc, xTitulo, yTitulo, bmpTitulo.bmWidth, bmpTitulo.bmHeight, bmpDCTitulo, 0, 0, SRCCOPY);
			BitBlt(hdc, xBackground, yBackground, bmpBackground.bmWidth, bmpBackground.bmHeight, bmpDCBackground, 0, 0, SRCCOPY);
			if (pLocal != nullptr) {
				for (int i = 0; i < pLocal->numeroFaixas * 20; ++i) {
					switch (pLocal->objetos[i].tipo) {
					case CARRO:
						BitBlt(hdc, xBackground + pLocal->objetos[i].posicaoX * DESLOCAMENTO, yBackground + pLocal->objetos[i].posicaoY * DESLOCAMENTO, bmpCarro.bmWidth, bmpCarro.bmHeight, bmpDCCarro, 0, 0, SRCCOPY);
						break;
					case SAPO:
						BitBlt(hdc, xBackground + pLocal->objetos[i].posicaoX * DESLOCAMENTO, yBackground + pLocal->objetos[i].posicaoY * DESLOCAMENTO, bmpSapo.bmWidth, bmpSapo.bmHeight, bmpDCSapo, 0, 0, SRCCOPY);
						break;
					case OBSTACULO:
						BitBlt(hdc, xBackground + pLocal->objetos[i].posicaoX * DESLOCAMENTO, yBackground + pLocal->objetos[i].posicaoY * DESLOCAMENTO, bmpObstaculo.bmWidth, bmpObstaculo.bmHeight, bmpDCObstaculo, 0, 0, SRCCOPY);
						break;
					default:
						break;
					}
				}
			}
			for (int i = 0; i < pLocal->vidas; i++)
				BitBlt(hdc, i * xVidas, yVidas, bmpVidas.bmWidth, bmpVidas.bmHeight, bmpDCVidas, 0, 0, SRCCOPY);
			BitBlt(hdc, xBannerTempo, yBannerTempo, bmpBannerTempo.bmWidth, bmpBannerTempo.bmHeight, bmpDCBannerTempo, 0, 0, SRCCOPY);
			for (int i = 0; i < pLocal->tempo; i++)
				BitBlt(hdc, xBarraTempo - i * bmpBarraTempo.bmWidth, yBarraTempo, bmpBarraTempo.bmWidth, bmpBarraTempo.bmHeight, bmpDCBarraTempo, 0, 0, SRCCOPY);
		} break;
		case 1: {
			BitBlt(hdc, xTitulo, yTitulo, bmpTitulo.bmWidth, bmpTitulo.bmHeight, bmpDCTitulo, 0, 0, SRCINVERT);
			BitBlt(hdc, xBackground, yBackground, bmpBackground.bmWidth, bmpBackground.bmHeight, bmpDCBackground, 0, 0, SRCINVERT);
			if (pLocal != nullptr) {
				for (int i = 0; i < pLocal->numeroFaixas * 20; ++i) {
					switch (pLocal->objetos[i].tipo) {
					case CARRO:
						BitBlt(hdc, xBackground + pLocal->objetos[i].posicaoX * DESLOCAMENTO, yBackground + pLocal->objetos[i].posicaoY * DESLOCAMENTO, bmpCarro.bmWidth, bmpCarro.bmHeight, bmpDCCarro, 0, 0, SRCINVERT);
						break;
					case SAPO:
						BitBlt(hdc, xBackground + pLocal->objetos[i].posicaoX * DESLOCAMENTO, yBackground + pLocal->objetos[i].posicaoY * DESLOCAMENTO, bmpSapo.bmWidth, bmpSapo.bmHeight, bmpDCSapo, 0, 0, SRCINVERT);
						break;
					case OBSTACULO:
						BitBlt(hdc, xBackground + pLocal->objetos[i].posicaoX * DESLOCAMENTO, yBackground + pLocal->objetos[i].posicaoY * DESLOCAMENTO, bmpObstaculo.bmWidth, bmpObstaculo.bmHeight, bmpDCObstaculo, 0, 0, SRCINVERT);
						break;
					default:
						break;
					}
				}
			}
			for (int i = 0; i < pLocal->vidas; i++)
				BitBlt(hdc, i * xVidas, yVidas, bmpVidas.bmWidth, bmpVidas.bmHeight, bmpDCVidas, 0, 0, SRCINVERT);
			BitBlt(hdc, xBannerTempo, yBannerTempo, bmpBannerTempo.bmWidth, bmpBannerTempo.bmHeight, bmpDCBannerTempo, 0, 0, SRCINVERT);
			for (int i = 0; i < pLocal->tempo; i++)
				BitBlt(hdc, xBarraTempo - i * bmpBarraTempo.bmWidth, yBarraTempo, bmpBarraTempo.bmWidth, bmpBarraTempo.bmHeight, bmpDCBarraTempo, 0, 0, SRCINVERT);
		}
		}

		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		// obter o tamanho da janela
		GetClientRect(hWnd, &rect);
		// alterar a posição do bitmap do background
		xBackground = (rect.right - rect.left - bmpBackground.bmWidth) / 2;
		yBackground = (rect.bottom - rect.top - bmpBackground.bmHeight) / 2;
		// alterar a posição do bitmap do obstaculo em relação ao background
		xObstaculo = xBackground + 4;
		yObstaculo = yBackground + bmpObstaculo.bmHeight + 2;
		// alterar a posição do bitmap do carro em relação ao background
		xCarro = xBackground + 4;
		yCarro = yBackground + (bmpCarro.bmHeight + 4) * 3;
		// alterar a posição do bitmap do sapo em relação ao background
		xSapo = xBackground + 4;
		ySapo = yBackground + bmpBackground.bmHeight - bmpSapo.bmHeight;
		// alterar a posição do bitmap do titulo em relação ao background
		xTitulo = xBackground;
		yTitulo = yBackground - bmpTitulo.bmHeight;
		// alterar a posição do bitmap das vidas em relação ao background
		xVidas = xBackground;
		yVidas = yBackground + bmpBackground.bmHeight;
		// alterar a posição do bitmap do banner do tempo em relação ao background
		xBannerTempo = xBackground + bmpBackground.bmWidth - bmpBannerTempo.bmWidth;
		yBannerTempo = yBackground + bmpBackground.bmHeight;
		// alterar a posição do bitmap da barra do tempo em relação ao background
		xBarraTempo = xBackground + bmpBackground.bmWidth - bmpBarraTempo.bmWidth;
		yBarraTempo = yBackground + bmpBackground.bmHeight + bmpBannerTempo.bmHeight;

		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			// caso a opção "Single player" do menu "Novo jogo" seja selecionada
		case IDM_SINGLE_PLAYER:
			/*
			// ler a informação da struct comum com os dados do jogo
			//WaitForSingleObject(hMutex, INFINITE);
			// obter o tamanho da janela
			GetClientRect(hWnd, &rect);
			// alterar a posição do bitmap do background para o colocar numa posição inicial no centro da janela
			xBackground = (rect.right - rect.left - bmpBackground.bmWidth) / 2;
			yBackground = (rect.bottom - rect.top - bmpBackground.bmHeight) / 2;
			// por cada obstaculo que existir na struct comum alterar a posição do bitmap do obstaculo em relação ao background
			for (int i = 0; i < 20; i++) {
				xObstaculo = xBackground + 4;
				yObstaculo = yBackground + bmpObstaculo.bmHeight + 2;
				xObstaculo += (bmpObstaculo.bmWidth + 4) * i;
			}
			*/
			//SetEvento(local.hEvento);
			MessageBox(hWnd, TEXT("Single Player selecionado"), TEXT("Confirmação"), MB_OK | MB_ICONERROR);
			break;
		case IDM_MULTI_PLAYER:
			MessageBox(hWnd, TEXT("Multi Player selecionado"), TEXT("Confirmação"), MB_OK | MB_ICONERROR);
			break;
		case IDM_EXIT:
			if (MessageBox(hWnd, TEXT("Deseja mesmo sair?"), TEXT("Sair"), MB_YESNO | MB_ICONQUESTION) == IDYES)
				DestroyWindow(hWnd);
			break;
		case IDM_ABOUT:
			MessageBox(hWnd, TEXT("Trabalho elaborado por Lourenço McBride e Filipe Carvalho"), TEXT("Sobre"), MB_OK | MB_ICONINFORMATION);
			break;
		case IDM_COR_LIGHT:
			theme = 2;
			break;
		case IDM_COR_DARK:
			theme = 1;
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



