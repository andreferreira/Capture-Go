#include <time.h>
#include "ia.h"
#include "GoHash.h"

#define INFINITY 0xffffff
#define STARTGAME 1

inline Board vizinhoCima(Board a) {
	return (a << 7) & FULLBOARD;
}

inline Board vizinhoBaixo(Board a) {
	return (a >> 7) & FULLBOARD;
}
inline Board vizinhoEsquerda(Board a) {
	return (a >> 1) & POSSUEM_VIZINHO_ESQUERDA;
}
inline Board vizinhoDireita(Board a) {
	return (a << 1) & POSSUEM_VIZINHO_DIREITA;
}

//Board vizinhosTable[] = {130LL ,261 ,522 ,1044 ,2088 ,4176 ,8224 ,16641 ,33410 ,66820 ,133640 ,267280 ,534560 ,1052736 ,2130048 ,4276480 ,8552960 ,17105920 ,34211840 ,68423680 ,134750208 ,272646144 ,547389440 ,1094778880 ,2189557760 ,4379115520LL ,8758231040LL ,17248026624LL ,34898706432LL ,70065848320LL ,140131696640LL ,280263393280LL ,560526786560LL ,1121053573120LL ,2207747407872LL ,4467034423296LL ,8968428584960LL ,17936857169920LL ,35873714339840LL ,71747428679680LL ,143494857359360LL ,282591668207616LL ,8830452760576LL ,22058952032256LL ,44117904064512LL ,88235808129024LL ,176471616258048LL ,352943232516096LL ,142936511610880LL};
inline Board vizinhos(Board a) {
	//return vizinhosTable[bitcount(a-1)];
	return vizinhoCima(a) | vizinhoBaixo(a) | vizinhoEsquerda(a) | vizinhoDireita(a);
}


Board startgametable(Board IA, Board opponent) {
	return 1 << 24;
}


void territorios(Board a, Board b, Board &territorioA, Board &territorioB, Board &territorioNeutro) {
	territorioA = a;
	territorioB = b;
	Board velhoA, velhoB;
	do {
		velhoA = territorioA;
		velhoB = territorioB;
		Board novoA, novoB;
		novoA = territorioA;
		novoB = territorioB;
		for (Board i = 1; i < BOARDMAX; i <<= 1) {
			if (i & territorioA)
				novoA |= vizinhos(i) & ~b & ~territorioB;
			if (i & territorioB)
				novoB |= vizinhos(i) & ~a & ~territorioA;	
		}
		Board neutro = novoA & novoB;
		territorioA = novoA & ~neutro;
		territorioB = novoB & ~neutro;
	} while (velhoA != territorioA || velhoB != territorioB);
	territorioNeutro = ~(territorioA | territorioB) & FULLBOARD;
}

int contaGruposDiagonal(Board a, Board b) {
	int total = 0;
	//debugBoard(a);
	//debugBoard(b);
	while (a) {
		Board seed = (a & (a-1)) ^ a;
		Board grupoAtual = seed;
		Board filaA = seed;
		Board visitados = 0;
		while (filaA) {
			Board topo = (filaA & (filaA-1)) ^ filaA;
			visitados |= topo;
			filaA = filaA & ~topo;
			grupoAtual |= a & vizinhos(topo);
			if (!((b & vizinhoEsquerda(topo)) | (b & vizinhoCima(topo))))
				grupoAtual |= a & vizinhoEsquerda(vizinhoCima(topo));
			if (!((b & vizinhoDireita(topo)) | (b & vizinhoCima(topo))))
				grupoAtual |= a & vizinhoDireita(vizinhoCima(topo));
			if (!((b & vizinhoEsquerda(topo)) | (b & vizinhoBaixo(topo))))
				grupoAtual |= a & vizinhoEsquerda(vizinhoBaixo(topo));
			if (!((b & vizinhoDireita(topo)) | (b & vizinhoBaixo(topo))))
				grupoAtual |= a & vizinhoDireita(vizinhoBaixo(topo));
			filaA |= grupoAtual & ~visitados;
		}
		a = a & ~grupoAtual;
		total++;
	}
	//std::cout<<total<<std::endl;
	//std::cin.get();
	return total;
}

int contaGrupos(Board a, Board b) {
	int total = 0;
	while (a) {
		Board seed = (a & (a-1)) ^ a;
		Board grupoAtual = seed;
		Board filaA = seed;
		Board visitados = 0;
		while (filaA) {
			Board topo = (filaA & (filaA-1)) ^ filaA;
			visitados |= topo;
			filaA = filaA & ~topo;
			grupoAtual |= a & vizinhos(topo);
			filaA |= grupoAtual & ~visitados;
		}
		a = a & ~grupoAtual;
		total++;
	}
	return total;
}


//unsigned long long folhas = 0; //para testes

//funçao de avaliaçao, precisa melhorar
int heuristic(Board max, Board min) {
	//folhas++;
	Board territorioMax,territorioMin, territorioNeutro;
	territorios(max,min,territorioMax,territorioMin,territorioNeutro);
	int valueTerritorio = bitcount(territorioMax)-bitcount(territorioMin);//-(bitcount(territorioNeutro)/2);
	char gruposMax = contaGrupos(max,min);
	char gruposMin = contaGrupos(min,max);
	char valueGrupos = gruposMin - gruposMax;
	return valueTerritorio + 100*valueGrupos;
}

Board visitaVizinhos(Board board) {
	Board result = 0;
	for (Board i = 1; i < BOARDMAX; i <<= 1) {
		if (i & board)
			result |= vizinhos(i);
	}
	return result;
}

GoHash * cache = NULL;

void inicializaCache() {
	 delete[] cache;
     cache = new GoHash[MAX_LEVEL];
	 inicializaRandombits();
}

inline void nucleoAlphabeta(Board max, Board min, Board fila, int &alpha, int &beta, char depth, Board &movement, char maxLevel) {
    Board i;
    Board ignore; //nao eh nescessario saber qual movimento, soh seu valor
    if (beta <= alpha)
        return;
	while (fila) {
		i = (fila & (fila-1)) ^ fila; //i aponta para bit menos significativo da fila, a jogada que iremos tentar
		fila = fila & ~i; //tira a jogada da fila
		int childvalue = -alphabeta(min,max|i,depth-1,-beta,-alpha,ignore,maxLevel);
			if (childvalue > alpha) {
				alpha = childvalue;
				movement = i;
			}
			if (beta <= alpha)
				break;
	} 
}



int alphabeta(Board max, Board min,char depth, int alpha, int beta, Board &movement, char maxLevel) {
	char victory = victoryTestMinMax(min,max,MIN);
	if (victory == MAX)
		return INFINITY;
	else if (victory == MIN)
		return -INFINITY;
	if (depth == 0)
		return heuristic(max,min);
	if (maxLevel-depth > 2) {
	int cacheval = cache[maxLevel-depth].get(max,min,maxLevel);
	
	if (cacheval != NOT_FOUND)
	   return cacheval;
    } 
	
	//usar tabela de transposiçao aqui
	movement = ~(max | min)& FULLBOARD;
	movement = (movement & (movement-1)) ^ movement; //essas linhas obrigam um jogador a sempre fazer uma jogada valida. Testar se sao nescessarias
	
    static Board killermoves[MAX_LEVEL] = {0};
    killermoves[depth] = killermoves[depth] & ~(max|min);
    Board prioridade = visitaVizinhos(max);
	prioridade = prioridade & ~(min|max|killermoves[depth]);
	Board prioridade2 = visitaVizinhos(min);
	prioridade2 = prioridade2 & ~(min|max|prioridade|killermoves[depth]);
	Board visita = ~(max|min) & FULLBOARD;
	visita = visita & ~(prioridade|prioridade2|killermoves[depth]);
	
	//if ((prioridade | visita | prioridade2| killermoves[depth]) != (~(max|min) & FULLBOARD))
	//	std::cout<<"ERRO"<<std::endl;
	
	
		
	nucleoAlphabeta(max,min,killermoves[depth],alpha,beta,depth,movement,maxLevel);
	nucleoAlphabeta(max,min,prioridade,alpha,beta,depth,movement,maxLevel);
	nucleoAlphabeta(max,min,prioridade2,alpha,beta,depth,movement,maxLevel);
	nucleoAlphabeta(max,min,visita,alpha,beta,depth,movement,maxLevel);
	killermoves[depth] = movement;
	if (maxLevel-depth > 2)	
		cache[maxLevel-depth].insert(max,min,alpha,maxLevel);
	return alpha;
}

Board AIchooseMovement(Board IA, Board opponent, int dificuldade) {
	for (Board i= 1; i < BOARDMAX;i <<= 1) {
		if (i & ~(IA|opponent) && victoryTestMinMax(opponent,IA | i,MAX) == MAX) {
			//std::cout<<"Mate!"<<std::endl;
			return i;
		}
	}
	Board movement = ~(IA | opponent)& FULLBOARD;
	movement = (movement & (movement-1)) ^ movement; //essas 2 linhas sao para sempre jogar um movimento, mesmo quando iremos perder
	int turno = (bitcount(IA)+bitcount(opponent)); //utilizar turno para decidir quantos niveis iremos verificar?
	if (turno < STARTGAME) return startgametable(IA,opponent);
	
	
	//folhas = 0;
	clock_t start = clock();
	clock_t end = clock();
	int level = dificuldade;
	if (dificuldade == 0)
	   level = 5;
	int resultado = 0;
	while (end-start < 1000 && level < MAX_LEVEL && resultado != -INFINITY && resultado != INFINITY) { 
	      resultado = alphabeta(IA,opponent,level,-INFINITY,INFINITY,movement,level);
	      end = clock();
	      //std::cout<<"Turno:"<<turno/2<<" Level:"<<level<<" Folhas:"<<folhas<<" Resultado:"<<resultado<<" Time:"<<end-start<<std::endl; // para testes
          level++;
          if (dificuldade != 0)
             break;
    }
	if (resultado == -INFINITY) {
		Board newmovement = movement;
		for (int miope = level-2; miope >= 1; --miope) {
			resultado = alphabeta(IA,opponent,miope,-INFINITY,INFINITY,newmovement,miope);
			//std::cout<<"Iria perder, tentando com level:"<<miope<<std::endl;
			if (resultado != - INFINITY)
				return newmovement;
		}
	}
    return movement;
}

//retorna true caso a ganhou o jogo
bool checkVictory(Board a, Board b) {
	Board libertiesB;
	libertiesB = ~(a|b);
	Board estabilizou;
	//otimizar? tentar evitar caso no qual propaga apenas uma peça por iteraçao?
	do {
		estabilizou = libertiesB;
		Board i;
		int acesso = 0;
		for (i = 1; i < BOARDMAX; i <<= 1)
			if (i & libertiesB) {
				libertiesB |= vizinhos(i) & b;
			}
	} while (libertiesB != estabilizou);
	if (b & ~libertiesB)
		return true;
	else
		return false;
}

bool checkVictory2(Board a, Board b) {
	Board libertiesB;
	libertiesB = ~(a|b) & FULLBOARD;
	Board fila = libertiesB;
	Board visitados = 0;
	while (fila) {
		Board novafila = fila & (fila-1);
		Board acesso = (novafila ^ fila);
		Board salva = vizinhos(acesso) & b;
		libertiesB |= salva;
		visitados |= acesso;
		fila = (novafila | salva) & ~visitados;
	}
	if (b & ~libertiesB)
		return true;
	else
		return false;
}

char victoryTestMinMax(Board min,Board max,char lastPlayed) {
	min = min & FULLBOARD;
	max = max & FULLBOARD;
	bool maxWon,minWon;
	maxWon = checkVictory2(max,min);
	minWon = checkVictory2(min,max);
	//if (maxWon != checkVictory(max,min) || minWon != checkVictory(min,max))
	//	std::cout<<"ERRO NA FUNCAO DE TERMINO:"<<min<<" "<<max<<std::endl;
	if (maxWon && minWon)
		return lastPlayed;
	if (maxWon)
		return MAX;
	if (minWon)
		return MIN;
	return EMPTY;
}

char victoryTestBlackWhite(Board black,Board white,char lastPlayed) {
	if (lastPlayed == BLACK)
		lastPlayed = MAX;
	else
		lastPlayed = MIN;
	int winner = victoryTestMinMax(white,black,lastPlayed);
	if (winner == MAX)
		return BLACK;
	if (winner == MIN)
		return WHITE;
	return EMPTY;
}
