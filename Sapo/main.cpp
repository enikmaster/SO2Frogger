#include "Header.h"

// função para ler a mensagens do named pipe

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
		DWORD errorCode = GetLastError();
		TCHAR errorMessage[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, errorMessage, 256, NULL);
		MessageBox(NULL, errorMessage, TEXT("Erro na abertura do pipe"), MB_OK);
		ExitProcess(-1);
	}
	ConnectNamedPipe(hPipe, NULL);
	WaitNamedPipe(PIPE_NAME, NMPWAIT_USE_DEFAULT_WAIT);
	
	
	LOCAL local;
	local.vidas = 3;
	local.pontuacao = 0;
	local.nivel = 0;
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
		800,					// Altura default
		(HWND)HWND_DESKTOP,		// Janela-pai
		(HMENU)NULL,			// Menu
		(HINSTANCE)hInst,		// Instância
		NULL);				// Dados de criação
	if (!hWnd)
		return FALSE; // Se não for bem sucedido, termina o programa
	local.hWnd = hWnd;
	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)&local);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)lerMessages, (LPVOID)&local, 0, NULL); 
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)enviaComandos, (LPVOID)&local, 0, NULL);
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)enviaMensagens, (LPVOID)&local, 0, NULL); //Thread Mensagem	string direita ou esquerda ou cima ou baixo
	// ============================================================================
	// 4. Mostra a janela
	// ============================================================================
	SetFocus(hWnd);
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
	TCHAR tecla;
	// Retrieve the address of the 'local' variable associated with the window handle
	pLocal = (LOCAL*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	static int theme = 1; // 1 = Dark, 2 = Light

	static HDC bmpDCBackground = NULL, bmpDCBackgroundL = NULL;
	static HDC bmpDCSapo = NULL, bmpDCSapoL = NULL;
	static HDC bmpDCCarro = NULL, bmpDCCarroL = NULL;
	static HDC bmpDCObstaculo = NULL, bmpDCObstaculoL = NULL;
	static HDC bmpDCBannerTempo = NULL, bmpDCBannerTempoL = NULL;
	static HDC bmpDCBarraTempo = NULL, bmpDCBarraTempoL = NULL;
	static HDC bmpDCVidas = NULL, bmpDCVidasL = NULL;
	static HDC bmpDCTitulo = NULL, bmpDCTituloL = NULL;

	static HBITMAP hBmpBackground = NULL, hBmpBackgroundL = NULL;
	static HBITMAP hBmpSapo = NULL, hBmpSapoL = NULL;
	static HBITMAP hBmpCarro = NULL, hBmpCarroL = NULL;
	static HBITMAP hBmpObstaculo = NULL, hBmpObstaculoL = NULL;
	static HBITMAP hBmpBannerTempo = NULL, hBmpBannerTempoL = NULL;
	static HBITMAP hBmpBarraTempo = NULL, hBmpBarraTempoL = NULL;
	static HBITMAP hBmpVidas = NULL, hBmpVidasL = NULL;
	static HBITMAP hBmpTitulo = NULL, hBmpTituloL = NULL;

	static BITMAP bmpBackground, bmpBackgroundL;
	static BITMAP bmpSapo, bmpSapoL;
	static BITMAP bmpCarro, bmpCarroL;
	static BITMAP bmpObstaculo, bmpObstaculoL;
	static BITMAP bmpBannerTempo, bmpBannerTempoL;
	static BITMAP bmpBarraTempo, bmpBarraTempoL;
	static BITMAP bmpVidas, bmpVidasL;
	static BITMAP bmpTitulo, bmpTituloL;

	static int xBackground, yBackground, xBackgroundL, yBackgroundL;
	static int xSapo, ySapo, xSapoL, ySapoL;
	static int xCarro, yCarro, xCarroL, yCarroL;
	static int xObstaculo, yObstaculo, xObstaculoL, yObstaculoL;
	static int xBannerTempo, yBannerTempo, xBannerTempoL, yBannerTempoL;
	static int xBarraTempo, yBarraTempo, xBarraTempoL, yBarraTempoL;
	static int xVidas, yVidas, xVidasL, yVidasL;
	static int xTitulo, yTitulo, xTituloL, yTituloL;
	
	static HANDLE hMutex;
	switch (messg) {
	case WM_CREATE:
	{// criar um menu com opções de single player e multiplayer
		HMENU hMenu, hSubMenu;
		hMenu = CreateMenu();
		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, IDM_SINGLE_PLAYER, TEXT("Single Player"));
		AppendMenu(hSubMenu, MF_STRING, IDM_MULTI_PLAYER, TEXT("Multiplayer"));
		AppendMenu(hSubMenu, MF_STRING, IDM_PAUSE_GAME, TEXT("Pausa Jogo"));
		AppendMenu(hSubMenu, MF_STRING, IDM_RESUME_GAME, TEXT("Retomar Jogo"));
		AppendMenu(hSubMenu, MF_STRING, IDM_RESTART_GAME, TEXT("Reset Jogo"));
		AppendMenu(hSubMenu, MF_STRING, IDM_EXIT, TEXT("Exit"));
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("Jogo"));
		SetMenu(hWnd, hMenu);
		// criar um menu Opções com opções de cor
		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, IDM_COR_LIGHT, TEXT("Claro"));
		AppendMenu(hSubMenu, MF_STRING, IDM_COR_DARK, TEXT("Escuro"));
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
	hBmpBackgroundL = (HBITMAP)LoadImage(NULL, TEXT("backgroundL.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpBackgroundL == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do background!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpObstaculo = (HBITMAP)LoadImage(NULL, TEXT("obstaculo.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpObstaculo == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do obstaculo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpObstaculoL = (HBITMAP)LoadImage(NULL, TEXT("obstaculoL.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpObstaculoL == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do obstaculo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpCarro = (HBITMAP)LoadImage(NULL, TEXT("carro.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpCarro == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do carro!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpCarroL = (HBITMAP)LoadImage(NULL, TEXT("carroL.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpCarroL == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do carro!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpSapo = (HBITMAP)LoadImage(NULL, TEXT("sapo.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpSapo == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do sapo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpSapoL = (HBITMAP)LoadImage(NULL, TEXT("sapoL.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpSapoL == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do sapo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpBannerTempo = (HBITMAP)LoadImage(NULL, TEXT("tempo.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpBannerTempo == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do banner do tempo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpBannerTempoL = (HBITMAP)LoadImage(NULL, TEXT("tempoL.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpBannerTempoL == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do banner do tempo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpBarraTempo = (HBITMAP)LoadImage(NULL, TEXT("timer.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpBarraTempo == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap da barra do tempo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpBarraTempoL = (HBITMAP)LoadImage(NULL, TEXT("timerL.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpBarraTempoL == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap da barra do tempo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpVidas = (HBITMAP)LoadImage(NULL, TEXT("vida.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpVidas == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap das vidas!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpVidasL = (HBITMAP)LoadImage(NULL, TEXT("vidaL.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpVidasL == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap das vidas!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpTitulo = (HBITMAP)LoadImage(NULL, TEXT("title.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpTitulo == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do titulo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	hBmpTituloL = (HBITMAP)LoadImage(NULL, TEXT("titleL.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmpTituloL == NULL) {
		MessageBox(hWnd, TEXT("Não foi possível carregar o bitmap do titulo!"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return -1;
	}
	// obter os tamanhos dos diferentes bitmaps
	GetObject(hBmpBackground, sizeof(BITMAP), &bmpBackground);
	GetObject(hBmpBackgroundL, sizeof(BITMAP), &bmpBackgroundL);
	GetObject(hBmpObstaculo, sizeof(BITMAP), &bmpObstaculo);
	GetObject(hBmpObstaculoL, sizeof(BITMAP), &bmpObstaculoL);
	GetObject(hBmpCarro, sizeof(BITMAP), &bmpCarro);
	GetObject(hBmpCarroL, sizeof(BITMAP), &bmpCarroL);
	GetObject(hBmpSapo, sizeof(BITMAP), &bmpSapo);
	GetObject(hBmpSapoL, sizeof(BITMAP), &bmpSapoL);
	GetObject(hBmpBannerTempo, sizeof(BITMAP), &bmpBannerTempo);
	GetObject(hBmpBannerTempoL, sizeof(BITMAP), &bmpBannerTempoL);
	GetObject(hBmpBarraTempo, sizeof(BITMAP), &bmpBarraTempo);
	GetObject(hBmpBarraTempoL, sizeof(BITMAP), &bmpBarraTempoL);
	GetObject(hBmpVidas, sizeof(BITMAP), &bmpVidas);
	GetObject(hBmpVidasL, sizeof(BITMAP), &bmpVidasL);
	GetObject(hBmpTitulo, sizeof(BITMAP), &bmpTitulo);
	GetObject(hBmpTituloL, sizeof(BITMAP), &bmpTituloL);
	
	hMutex = CreateMutex(NULL, FALSE, NULL);
	// ########################################### Background Dark ###########################################
	hdc = GetDC(hWnd);
	bmpDCBackground = CreateCompatibleDC(hdc);
	SelectObject(bmpDCBackground, hBmpBackground);
	GetClientRect(hWnd, &rect);
	xBackground = 0;
	yBackground = rect.bottom - bmpBackground.bmHeight;
	// ########################################### Background Light ###########################################
	hdc = GetDC(hWnd);
	bmpDCBackgroundL = CreateCompatibleDC(hdc);
	SelectObject(bmpDCBackgroundL, hBmpBackgroundL);
	GetClientRect(hWnd, &rect);
	xBackgroundL = 0;
	yBackgroundL = rect.bottom - bmpBackgroundL.bmHeight;
	// ########################################### Obstaculo Dark ###########################################
	bmpDCObstaculo = CreateCompatibleDC(hdc);
	SelectObject(bmpDCObstaculo, hBmpObstaculo);
	GetClientRect(hWnd, &rect);
	xObstaculo = 0;
	yObstaculo = 4 + bmpObstaculo.bmHeight;
	// ########################################### Obstaculo Light ###########################################
	bmpDCObstaculoL = CreateCompatibleDC(hdc);
	SelectObject(bmpDCObstaculoL, hBmpObstaculoL);
	GetClientRect(hWnd, &rect);
	xObstaculoL = 0;
	yObstaculoL = 4 + bmpObstaculoL.bmHeight;
	// ########################################### Carro ###########################################
	bmpDCCarro = CreateCompatibleDC(hdc);
	SelectObject(bmpDCCarro, hBmpCarro);
	GetClientRect(hWnd, &rect);
	xCarro = 0;
	yCarro = 50;
	// ########################################### Carro Light ###########################################
	bmpDCCarroL = CreateCompatibleDC(hdc);
	SelectObject(bmpDCCarroL, hBmpCarroL);
	GetClientRect(hWnd, &rect);
	xCarroL = 0;
	yCarroL = 50;
	// ########################################### Sapo ###########################################
	bmpDCSapo = CreateCompatibleDC(hdc);
	SelectObject(bmpDCSapo, hBmpSapo);
	GetClientRect(hWnd, &rect);
	xSapo = xBackground + 4;
	ySapo = bmpBackground.bmHeight - bmpSapo.bmHeight - 2;
	// ########################################### Sapo Light ###########################################
	bmpDCSapoL = CreateCompatibleDC(hdc);
	SelectObject(bmpDCSapoL, hBmpSapoL);
	GetClientRect(hWnd, &rect);
	xSapoL = xBackgroundL + 4;
	ySapoL = bmpBackgroundL.bmHeight - bmpSapoL.bmHeight - 2;
	// ########################################### Banner Tempo ###########################################
	bmpDCBannerTempo = CreateCompatibleDC(hdc);
	SelectObject(bmpDCBannerTempo, hBmpBannerTempo);
	GetClientRect(hWnd, &rect);
	xBannerTempo = xBackground + bmpBackground.bmWidth - bmpBannerTempo.bmWidth;
	yBannerTempo = yBackground + bmpBackground.bmHeight;
	// ########################################### Banner Tempo Light ###########################################
	bmpDCBannerTempoL = CreateCompatibleDC(hdc);
	SelectObject(bmpDCBannerTempoL, hBmpBannerTempoL);
	GetClientRect(hWnd, &rect);
	xBannerTempoL = xBackgroundL + bmpBackgroundL.bmWidth - bmpBannerTempoL.bmWidth;
	yBannerTempoL = yBackgroundL + bmpBackgroundL.bmHeight;
	// ########################################### Barra Tempo ###########################################
	bmpDCBarraTempo = CreateCompatibleDC(hdc);
	SelectObject(bmpDCBarraTempo, hBmpBarraTempo);
	GetClientRect(hWnd, &rect);
	xBarraTempo = xBannerTempo - 4;
	yBarraTempo = yBannerTempo;
	// ########################################### Barra Tempo Light ###########################################
	bmpDCBarraTempoL = CreateCompatibleDC(hdc);
	SelectObject(bmpDCBarraTempoL, hBmpBarraTempoL);
	GetClientRect(hWnd, &rect);
	xBarraTempoL = xBannerTempoL - 4;
	yBarraTempoL = yBannerTempoL;
	// ########################################### Vidas ###########################################
	bmpDCVidas = CreateCompatibleDC(hdc);
	SelectObject(bmpDCVidas, hBmpVidas);
	GetClientRect(hWnd, &rect);
	xVidas = xBackground;
	yVidas = yBackground + bmpBackground.bmHeight;
	// ########################################### Vidas Light ###########################################
	bmpDCVidasL = CreateCompatibleDC(hdc);
	SelectObject(bmpDCVidasL, hBmpVidasL);
	GetClientRect(hWnd, &rect);
	xVidasL = xBackgroundL;
	yVidasL = yBackgroundL + bmpBackgroundL.bmHeight;
	// ########################################### Titulo ###########################################
	bmpDCTitulo = CreateCompatibleDC(hdc);
	SelectObject(bmpDCTitulo, hBmpTitulo);
	GetClientRect(hWnd, &rect);
	xTitulo = xBackground;
	yTitulo = yBackground - bmpTitulo.bmHeight;
	// ########################################### Titulo Light ###########################################
	bmpDCTituloL = CreateCompatibleDC(hdc);
	SelectObject(bmpDCTituloL, hBmpTituloL);
	GetClientRect(hWnd, &rect);
	xTituloL = xBackgroundL;
	yTituloL = yBackgroundL - bmpTituloL.bmHeight;
	ReleaseDC(hWnd, hdc);

	break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		FillRect(hdc, &rect, CreateSolidBrush(RGB(0, 0, 0)));

		switch (theme) {
		case 1: {
			BitBlt(hdc, xTitulo, yTitulo, bmpTitulo.bmWidth, bmpTitulo.bmHeight, bmpDCTitulo, 0, 0, SRCCOPY);
			BitBlt(hdc, xBackground, yBackground, bmpBackground.bmWidth, bmpBackground.bmHeight, bmpDCBackground, 0, 0, SRCCOPY);
			BitBlt(hdc, xBannerTempo, yBannerTempo, bmpBannerTempo.bmWidth, bmpBannerTempo.bmHeight, bmpDCBannerTempo, 0, 0, SRCCOPY);
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
				for (int i = 0; i < pLocal->vidas; i++)
					BitBlt(hdc, i * bmpVidas.bmWidth + xVidas, yVidas, bmpVidas.bmWidth, bmpVidas.bmHeight, bmpDCVidas, 0, 0, SRCCOPY);
				for (int i = 0; i < pLocal->tempo; i++)
					BitBlt(hdc, xBarraTempo - i * bmpBarraTempo.bmWidth, yBarraTempo, bmpBarraTempo.bmWidth, bmpBarraTempo.bmHeight, bmpDCBarraTempo, 0, 0, SRCCOPY);
			}
		} break;
		case 2: {
			BitBlt(hdc, xTituloL, yTituloL, bmpTituloL.bmWidth, bmpTituloL.bmHeight, bmpDCTituloL, 0, 0, SRCCOPY);
			BitBlt(hdc, xBackgroundL, yBackgroundL, bmpBackgroundL.bmWidth, bmpBackgroundL.bmHeight, bmpDCBackgroundL, 0, 0, SRCCOPY);
			BitBlt(hdc, xBannerTempoL, yBannerTempoL, bmpBannerTempoL.bmWidth, bmpBannerTempoL.bmHeight, bmpDCBannerTempoL, 0, 0, SRCCOPY);
			if (pLocal != nullptr) {
				for (int i = 0; i < pLocal->numeroFaixas * 20; ++i) {
					switch (pLocal->objetos[i].tipo) {
					case CARRO:
						BitBlt(hdc, xBackgroundL + pLocal->objetos[i].posicaoX * DESLOCAMENTO, yBackgroundL + pLocal->objetos[i].posicaoY * DESLOCAMENTO, bmpCarroL.bmWidth, bmpCarroL.bmHeight, bmpDCCarroL, 0, 0, SRCCOPY);
						break;
					case SAPO:
						BitBlt(hdc, xBackgroundL + pLocal->objetos[i].posicaoX * DESLOCAMENTO, yBackgroundL + pLocal->objetos[i].posicaoY * DESLOCAMENTO, bmpSapoL.bmWidth, bmpSapoL.bmHeight, bmpDCSapoL, 0, 0, SRCCOPY);
						break;
					case OBSTACULO:
						BitBlt(hdc, xBackgroundL + pLocal->objetos[i].posicaoX * DESLOCAMENTO, yBackgroundL + pLocal->objetos[i].posicaoY * DESLOCAMENTO, bmpObstaculoL.bmWidth, bmpObstaculoL.bmHeight, bmpDCObstaculoL, 0, 0, SRCCOPY);
						break;
					default:
						break;
					}
				}
				for (int i = 0; i < pLocal->vidas; i++)
					BitBlt(hdc, i * bmpVidas.bmWidth + xVidasL, yVidasL, bmpVidasL.bmWidth, bmpVidasL.bmHeight, bmpDCVidasL, 0, 0, SRCCOPY);
				for (int i = 0; i < pLocal->tempo; i++)
					BitBlt(hdc, xBarraTempoL - i * bmpBarraTempoL.bmWidth, yBarraTempoL, bmpBarraTempoL.bmWidth, bmpBarraTempoL.bmHeight, bmpDCBarraTempoL, 0, 0, SRCCOPY);
			}
		} break;
		}
		TCHAR info[100];
		_stprintf_s(info, TEXT("Nivel: %d Pontuação: %d\0"), pLocal->nivel, pLocal->pontuacao);
		SetTextColor(hdc, RGB(255, 0, 0));
		SetBkMode(hdc, TRANSPARENT);
		rect.left = xBackground;
		rect.top = yBackground - 20;
		DrawText(hdc, info, (DWORD)(_tcslen(info)), &rect, DT_SINGLELINE | DT_NOCLIP);
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &rect);
		xBackgroundL = xBackground = (rect.right - rect.left - bmpBackground.bmWidth) / 2;
		yBackgroundL = yBackground = (rect.bottom - rect.top - bmpBackground.bmHeight) / 2;
		xObstaculoL = xObstaculo = xBackground + 4;
		yObstaculoL = yObstaculo = yBackground + bmpObstaculo.bmHeight + 2;
		xCarroL = xCarro = xBackground + 4;
		yCarroL = yCarro = yBackground + (bmpCarro.bmHeight + 4) * 3;
		xSapoL = xSapo = xBackground + 4;
		ySapoL = yBackground + bmpBackground.bmHeight - bmpSapo.bmHeight;
		xTituloL = xTitulo = xBackground;
		yTituloL = yTitulo = yBackground - bmpTitulo.bmHeight;
		xVidasL = xVidas = xBackground;
		yVidasL = yVidas = yBackground + bmpBackground.bmHeight;
		xBannerTempoL = xBannerTempo = xBackground + bmpBackground.bmWidth - bmpBannerTempo.bmWidth;
		yBannerTempoL = yBannerTempo = yBackground + bmpBackground.bmHeight;
		xBarraTempoL = xBarraTempo = xBackground + bmpBackground.bmWidth - bmpBarraTempo.bmWidth;
		yBarraTempoL = yBarraTempo = yBackground + bmpBackground.bmHeight + bmpBannerTempo.bmHeight;
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			// caso a opção "Single player" do menu "Novo jogo" seja selecionada
		case IDM_SINGLE_PLAYER:
			if(!pLocal->playing) {
				pLocal->playing = TRUE;
				wcscpy_s(pLocal->mensagemaEnviar, SINGLEPLAYEROPT);
				WriteFile(pLocal->hPipe, pLocal->mensagemaEnviar, (DWORD)(_tcslen(pLocal->mensagemaEnviar) + 1) * sizeof(TCHAR), &pLocal->nBytesWriten, NULL);
			}
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
		case IDM_PAUSE_GAME:
			MessageBox(hWnd, TEXT("Pausar"), TEXT("Confirmação"), MB_OK | MB_ICONERROR);
			wcscpy_s(pLocal->mensagemaEnviar, TEXT("7\0"));
			WriteFile(pLocal->hPipe, pLocal->mensagemaEnviar, (DWORD)(_tcslen(pLocal->mensagemaEnviar) + 1) * sizeof(TCHAR), &pLocal->nBytesWriten, NULL);
			if (pLocal->nBytesWriten < 1) {
				MessageBox(hWnd, TEXT("O Servidor encerrou a conexão"), TEXT("Confirmação"), MB_OK | MB_ICONERROR);
				ExitProcess(0);
			}
			Sleep(1000);
			break;
		case IDM_RESUME_GAME:
			MessageBox(hWnd, TEXT("Retomar"), TEXT("Confirmação"), MB_OK | MB_ICONERROR);
			wcscpy_s(pLocal->mensagemaEnviar, TEXT("8\0"));
			WriteFile(pLocal->hPipe, pLocal->mensagemaEnviar, (DWORD)(_tcslen(pLocal->mensagemaEnviar) + 1) * sizeof(TCHAR), &pLocal->nBytesWriten, NULL);
			if (pLocal->nBytesWriten < 1) {
				MessageBox(hWnd, TEXT("O Servidor encerrou a conexão"), TEXT("Confirmação"), MB_OK | MB_ICONERROR);
				ExitProcess(0);
			}
			break;
		case IDM_RESTART_GAME:
			MessageBox(hWnd, TEXT("Reiniciar"), TEXT("Confirmação"), MB_OK | MB_ICONERROR);
			break;
		}
		break;
	case WM_KEYDOWN:
		tecla = (TCHAR)wParam;
		switch (tecla) {
		case VK_UP:
			if (pLocal->myY == 0)
				break;
			if (pLocal->myY != 0 && pLocal->objetos[20 * pLocal->myY + pLocal->myX - 20].tipo == SAPO)
				break;
			wcscpy_s(pLocal->mensagemaEnviar, MOVE_UP);
			WriteFile(pLocal->hPipe, pLocal->mensagemaEnviar, (DWORD)(_tcslen(pLocal->mensagemaEnviar) + 1) * sizeof(TCHAR), &pLocal->nBytesWriten, NULL);;
			if (pLocal->nBytesWriten < 1) {
				MessageBox(hWnd, TEXT("O Servidor encerrou a conexão"), TEXT("Confirmação"), MB_OK | MB_ICONERROR);
				ExitProcess(0);
			}
			break;
		case VK_DOWN:
			if (pLocal->myY == 0 || pLocal->myY == pLocal->numeroFaixas)
				break;
			if (pLocal->myY != pLocal->numeroFaixas && pLocal->objetos[20 * pLocal->myY + pLocal->myX + 20].tipo == SAPO)
				break;
			wcscpy_s(pLocal->mensagemaEnviar, MOVE_DOWN);
			WriteFile(pLocal->hPipe, pLocal->mensagemaEnviar, (DWORD)(_tcslen(pLocal->mensagemaEnviar) + 1) * sizeof(TCHAR), &pLocal->nBytesWriten, NULL);
			if (pLocal->nBytesWriten < 1) {
				MessageBox(hWnd, TEXT("O Servidor encerrou a conexão"), TEXT("Confirmação"), MB_OK | MB_ICONERROR);
				ExitProcess(0);
			}
			break;
		case VK_RIGHT:
			if (pLocal->myY == 0)
				break;
			if(pLocal->myX == NUMERO_COLUNAS-1 || pLocal->objetos[20 * pLocal->myY + pLocal->myX + 1].tipo == SAPO)
				break;
			wcscpy_s(pLocal->mensagemaEnviar, MOVE_RIGHT);
			WriteFile(pLocal->hPipe, pLocal->mensagemaEnviar, (DWORD)(_tcslen(pLocal->mensagemaEnviar) + 1) * sizeof(TCHAR), &pLocal->nBytesWriten, NULL);
			if (pLocal->nBytesWriten < 1) {
				MessageBox(hWnd, TEXT("O Servidor encerrou a conexão"), TEXT("Confirmação"), MB_OK | MB_ICONERROR);
				ExitProcess(0);
			}
			break;
		case VK_LEFT:
			if (pLocal->myY == 0 || pLocal->myX == 0 || pLocal->objetos[20 * pLocal->myY + pLocal->myX - 1].tipo == SAPO)
				break;
			wcscpy_s(pLocal->mensagemaEnviar, MOVE_LEFT);
			WriteFile(pLocal->hPipe, pLocal->mensagemaEnviar, (DWORD)(_tcslen(pLocal->mensagemaEnviar) + 1) * sizeof(TCHAR), &pLocal->nBytesWriten, NULL);
			if (pLocal->nBytesWriten < 1) {
				MessageBox(hWnd, TEXT("O Servidor encerrou a conexão"), TEXT("Confirmação"), MB_OK | MB_ICONERROR);
				ExitProcess(0);
			}
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		// obter as coordenadas do clique
		// verificas as coordenadas do clique em relação Às coordenadas do sapo
		// se o clique for acima do sapo, envia Move Up local.mensagemaEnviar[0] = MOVE_UP;
		// se o clique for abaixo do sapo, envia Move Down
		// se o clique for à direita do sapo, envia Move Right
		// se o clique for à esquerda do sapo, envia Move Left
		// setEvent(local.hEventoEnviaMensagem);
		break;
	case WM_RBUTTONDOWN:
		// mover o sapo para a posição inicial
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
void ParseMessage(LOCAL* origenate) {
	if (wcslen(origenate->mensagem) == 0) {
		// mensagem vazia, encerrou o servidor
		return;
	}
	if (wcslen(origenate->mensagem) < 30) {
		// mensagem com informações complementares ao jogo (terminou, ganhou, encerrou)
	}
	else {
		TCHAR* next_token = NULL;
		TCHAR* token = _tcstok_s((TCHAR*)origenate->mensagem, TEXT(" "), &next_token);
		origenate->myY = _ttoi(token);
		token = _tcstok_s(NULL, TEXT(" "), &next_token);
		origenate->myX = _ttoi(token);
		token = _tcstok_s(NULL, TEXT(" "), &next_token);
		origenate->nivel = _ttoi(token);
		token = _tcstok_s(NULL, TEXT(" "), &next_token);
		origenate->vidas = _ttoi(token);
		token = _tcstok_s(NULL, TEXT(" "), &next_token);
		origenate->tempo = _ttoi(token);
		token = _tcstok_s(NULL, TEXT(" "), &next_token);
		origenate->pontuacao = _ttoi(token);

		token = _tcstok_s(NULL, TEXT("\0"), &next_token);
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
DWORD WINAPI lerMessages(LPVOID param) {
	LOCAL* origem = (LOCAL*)param;
	HANDLE hPipe = origem->hPipe;
	BOOL res;
	LOCAL x;
	HANDLE hEventos[2];
	hEventos[0] = hPipe;
	hEventos[1] = origem->hEventoEnviaMensagem;
	while (1)
	{
		DWORD dwWaitResult = WaitForMultipleObjects(2, hEventos, FALSE, INFINITE);
		if (dwWaitResult == WAIT_OBJECT_0) // Pipe is ready to read
		{
			ReadFile(hPipe, origem->mensagem, BUFSIZE, &origem->nBytesRead, NULL);
			if (x.nBytesRead > 0) {
				ResetEvent(hEventos[0]);
				//x = ParseMessage(x.mensagem);
				EnterCriticalSection(&origem->cs);
				ParseMessage(origem);
				LeaveCriticalSection(&origem->cs);
				InvalidateRect(origem->hWnd, // handle da janela
					NULL, // retangulo a atualizar (NULL = toda a janela)
					TRUE); // força a atualização da janela
			}
			else {
				break;
			}
		}
		else if (dwWaitResult == (WAIT_OBJECT_0 + 1)) // GoWriteEvent is signaled
		{
			// Reset the event
			ResetEvent(origem->hEventoEnviaMensagem);
			WriteFile(hPipe, origem->mensagemaEnviar, (DWORD)(_tcslen(origem->mensagemaEnviar) + 1) * sizeof(TCHAR), &origem->nBytesWriten, NULL);
			if (origem->nBytesWriten <= 0) {
				_tprintf_s(TEXT("WriteFile failed with %d.\n", GetLastError()));
				break;
			}
		}
		else if (dwWaitResult == WAIT_FAILED)
		{
			_tprintf_s(TEXT("WaitForMultipleObjects failed with %d.\n", GetLastError()));
			break;
		}
	}
	return 0;
}