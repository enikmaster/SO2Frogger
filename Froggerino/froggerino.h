// froggerino.h  contém as declarações das funções partilhadas do frogger
#pragma once

#ifdef FROGGERINO_EXPORTS
#define FROGGERINO_API __declspec(dllexport)
#else
#define FROGGERINO_API __declspec(dllimport)
#endif

#define programa "servidor"
#define TAM 50
#define key1is "Software\\TP"
#define NOMEPIPE TEXT("\\\\.\\pipe\\canal")
#define keyValueNameF "Faixas" 
#define keyValueNameV "Velocity"
#define ATUALIZA TEXT("ATUALIZA")
#define STOPALL TEXT("STOPALL")
#define COLUMNS 20
#define JANELAX 60 
#define JANELAY 40
#define BUFFER_SIZE 10
#define EVENTSHAREDMEM TEXT("EVENTOREAD")
#define MUTEXSHAREDMEM TEXT("HMUTEX")
#define CONNECTING_STATE 0 
#define READING_STATE 1 
#define WRITING_STATE 2 
#define INSTANCES 2
#define PIPE_TIMEOUT 50000
#define BUFSIZE 4096
#define NSAPOS 2
#define TERMINOUJOGO TEXT("TERMINOUJOGO")
#define TERINOUTEMPO TEXT("TERMINOUTEMPO")
#define VIDAS 3
#define NIVEISDEJOGO 5
#define TEMPO 25000

FROGGERINO_API typedef enum {
    CMD_PARAR,    // uma palavra + um inteiro > 0 && < total de faixas
    CMD_INVERTER, // uma palavra + um inteiro > 0 && < total de faixas
    CMD_ADICIONAR, // uma palavra + um inteiro > 0 && < total de faixas
    CMD_RETOMAR,
    CMD_SAIR,
    CMD_PAUSEALL,
    CMD_ERRO // comando inválido
} Comando;

FROGGERINO_API typedef struct _BUFFERCELL { // informação para ser lida pelo servidor
    unsigned int id;
    DWORD f1;
    DWORD f2;
} BufferCell;


// estruturas a usar entre os vários programas
FROGGERINO_API typedef struct FAIXAVELOCITY { // informação sobre a faixa
    DWORD faixa;
    DWORD velocity;
} FaixaVelocity;

FROGGERINO_API typedef struct POS { //posição para array, cada thread trada de escrever uma faixa
    unsigned int X;
    unsigned int Y;
} pos;

FROGGERINO_API typedef struct BOARDGAME { // tabuleiro de jogo
    TCHAR** gameBoardArray;
    pos d;
    HANDLE hMutexArray;
    HANDLE hTimerBoard;
} BoardGame;

FROGGERINO_API typedef struct OBJECTO { // objetos do tabuleiro
    TCHAR s; // sapo 
    TCHAR c; // carro
    TCHAR o; // objeto
} objs;
FROGGERINO_API typedef struct SAPO {
    BOOL activo;
    BOOL move;
    unsigned int vidas;
    unsigned int temp;
    unsigned int score;
    pos pos_atual;
    pos pos_inicial;
    TCHAR* name;
} SAPO;

FROGGERINO_API typedef struct NIVEL {
    unsigned int lvlActual;
    unsigned int nCarros;
    unsigned int velocidade;
    BOOL sentido; //true -> fals <-
    SAPO* sapos;
    unsigned int tempo;
} Nivel;
FROGGERINO_API typedef struct
{
    OVERLAPPED oOverlap;
    OVERLAPPED* zO;
    HANDLE hPipeInst;
    TCHAR chRequest[BUFSIZE];
    DWORD cbRead;
    TCHAR chReply[BUFSIZE];
    DWORD cbToWrite;
    DWORD dwState;
    BOOL fPendingIO;
    BOOL ligado;
    BOOL sucesso;
    HANDLE hEvent;
    HANDLE hEventoThread;
} PIPEINST, * LPPIPEINST;


FROGGERINO_API typedef struct Eventos_Mutexs {
    HANDLE hEventoAtualiza; //sinaliza para atualizar tabuleiro
    HANDLE hEventoEscreveSapos; //sinaliza para escrever sapos
    HANDLE hMutexArrayJogo; //mutex para array de jogo
    HANDLE hEventStart; //Vai ser sinalizado apenas depois de se saber se são dois ou 1 sapo
    HANDLE hStdout;
    HANDLE hEventoTerminouTempo; //Sinaliza thread que tempo terminou para aquele nivel e vai carregar novo nivel corrigir tempo
    HANDLE hEventoTerminouJogo; //Sinaliza thread que o jogo terminou
    HANDLE hEventoStopAll; //Encerrar tudo
    HANDLE hEventoRestartGame; //Reiniciar jogo
    HANDLE hEventoPausaJogo; //Pausa jogo
    CRITICAL_SECTION x;
}Eventos_Mutexs;
FROGGERINO_API typedef struct _SHAREDMEM { // memória partilhada
    unsigned int faixaMax;
    unsigned int p;
    unsigned int c;
    unsigned int wP; //posição escrita
    unsigned int rP; //posição read
    BufferCell buffer[BUFFER_SIZE];
    TCHAR gameShared[10][20];
} SharedMem;
FROGGERINO_API typedef struct ControlaPipes {
    int sapoAControlar;
    TCHAR** GameBoard;
    PIPEINST* pipeMgm;
    SAPO* saposa;
    SAPO* saposb;
    TCHAR chRequest[BUFSIZE];
    DWORD cbRead;
    TCHAR chReply[BUFSIZE];
    DWORD cbToWrite;
    BOOL singlePlayer;
    Eventos_Mutexs* gere;
    DWORD nFaixas;
    HANDLE ThreadsParaSapo[INSTANCES];
}ControlaPipes;
FROGGERINO_API typedef struct INFO { // Informação completa sobre o jogo
    TCHAR** arrayGame; //aqui feito
    DWORD nFaixaResp; //faixa de atuacao
    DWORD id; //id
    DWORD numFaixasTotal;
    DWORD* end; // end
    BOOL moving; //if true andar if false parar
    BOOL allMoving;
    BOOL colocaObjeto;
    BOOL sentidoFaixa;
    HANDLE hEventPause; //Pausa individual
    HANDLE hEventPauseGlobal; // Pausa e re sinaliza main thread que tem de atualizar o tabuleiro para operador e tambem sinaliza thread responsavel do sapo
    Nivel* nivel;
    Eventos_Mutexs* a;
    SAPO* sapos;
    objs o; //objetos
} Info;
FROGGERINO_API typedef struct _CONTROLDATA { // informação para controlo de fluxo de dados
    unsigned int id;
    HANDLE hMapFile;
    SharedMem* sharedMem;
    HANDLE hMutex;
    HANDLE hEvent;
    HANDLE hWriteSem; // n
    HANDLE hReadSem;  // 1
    Info* infoControl;
} ControlData;




// variáveis
FROGGERINO_API int x;
// FUNÇÕES
//// funções para ler das estruturas (getters)
//// Info

//// funções para escrever nas estruturas (setters)

//// outras funções
#ifdef __cplusplus
extern "C" {
#endif

    //FROGGERINO_API void showBG(TCHAR);

#ifdef __cplusplus
}
#endif