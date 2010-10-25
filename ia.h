#ifndef IA_H
#define IA_H
#include "board.h"
#include "bit.h"
#include "Game.h"
#define MAX_LEVEL 10

void territorios(Board a, Board b, Board &territorioA, Board &territorioB);
int contaGrupos(Board a, Board b);
Board vizinhos(Board a);
int alphabeta(Board max, Board min,char depth, int alpha, int beta, Board &movement,char maxLevel);
int heuristic(Board max, Board min);
Board AIchooseMovement(Board IA, Board opponent, int dificuldade);
//returns the winner
char victoryTestMinMax(Board min,Board max,char lastPlayed);
char victoryTestBlackWhite(Board black,Board white,char lastPlayed);
//checks if player a won
bool checkVictory(Board a, Board b);
bool checkVictory2(Board a, Board b);
void inicializaCache();
#endif
