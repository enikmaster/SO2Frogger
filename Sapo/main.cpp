#include <windows.h>
#include <tchar.h>
//#include <math.h>
//#include <stdio.h>
//#include <fcntl.h>
//#include <io.h>
#include <windowsx.h>

#define IDM_SINGLE_PLAYER 1001
#define IDM_MULTI_PLAYER 1002
#define IDM_ABOUT 1003
#define IDM_EXIT 1004

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);

TCHAR szProgName[] = TEXT("Sapo");

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	HWND hWnd;		// hWnd é o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg;		// MSG é uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp;	// WNDCLASSEX é uma estrutura cujos membros servem para definir as características da classe da janela

	// ============================================================================
	// 1. Definição das características da janela "wcApp" 
	//    (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
	// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX);      // Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;		        // Instância da janela actualmente exibida ("hInst" é parâmetro de WinMain e vem inicializada daí)
	wcApp.lpszClassName = szProgName;       // Nome da janela (neste caso = nome do programa)
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
		szProgName,				// Nome da janela
		TEXT("Sapo"),			// Título da janela
		WS_OVERLAPPEDWINDOW,	// Estilo da janela
		250,					// Posição x default
		250,					// Posição y default
		800,					// Comprimento default
		500,					// Altura default
		(HWND)HWND_DESKTOP,		// Janela-pai
		(HMENU)NULL,			// Menu
		(HINSTANCE)hInst,		// Instância
		0);						// Dados de criação

	if (!hWnd)
		return FALSE; // Se não for bem sucedido, termina o programa

	// ============================================================================
	// 4. Mostra a janela
	// ============================================================================
	ShowWindow(hWnd, nCmdShow); // Mostra a janela
	UpdateWindow(hWnd);			// Desenha a janela

	// ============================================================================
	// 5. Processamento de mensagens
	// ============================================================================
	while (GetMessage(&lpMsg, NULL, 0, 0)) { // Processa as mensagens
		TranslateMessage(&lpMsg);	// Pré-processamento das mensagens
		DispatchMessage(&lpMsg);	// Envia as mensagens
	}
	return (int)lpMsg.wParam;		// Devolve o parâmetro "wParam" da estrutura "lpMsg"
}

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;

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
	{// criar um menu com opções de single player e multiplayer
		HMENU hMenu, hSubMenu;
		hMenu = CreateMenu();
		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, IDM_SINGLE_PLAYER, TEXT("Single Player"));
		AppendMenu(hSubMenu, MF_STRING, IDM_MULTI_PLAYER, TEXT("Multiplayer"));
		AppendMenu(hSubMenu, MF_STRING, IDM_EXIT, TEXT("Exit"));
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("Jogo"));
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
	// libertar o DC
	ReleaseDC(hWnd, hdc);

	break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		FillRect(hdc, &rect, CreateSolidBrush(RGB(55, 55, 55)));
		BitBlt(hdc, xBackground, yBackground, bmpBackground.bmWidth, bmpBackground.bmHeight, bmpDCBackground, 0, 0, SRCCOPY);
		for (int i = 0; i < 20; i++) {
			BitBlt(hdc, xObstaculo, yObstaculo, bmpObstaculo.bmWidth, bmpObstaculo.bmHeight, bmpDCObstaculo, 0, 0, SRCCOPY);
			xObstaculo += (bmpObstaculo.bmWidth + 4);
		}
		BitBlt(hdc, xCarro, yCarro, bmpCarro.bmWidth, bmpCarro.bmHeight, bmpDCCarro, 0, 0, SRCCOPY);
		BitBlt(hdc, xSapo, ySapo, bmpSapo.bmWidth, bmpSapo.bmHeight, bmpDCSapo, 0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
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

		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			// caso a opção "Single player" do menu "Novo jogo" seja selecionada
		case IDM_SINGLE_PLAYER:
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