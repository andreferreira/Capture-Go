#ifndef BOARD_H
#define BOARD_H
typedef unsigned long long Board;

#define xyTo64(x,y) ((Board)1 << (7*(y )+(x )))
#define getBit(board,x,y) ((Board)(board) & xyTo64(x,y)) 
#define setBit(board,x,y) ((board) | xyTo64(x,y))
#define unsetBit(board,x,y) ((board) & ~xyTo64(x,y))
#define BOARDMAX (xyTo64(6,6)<<1)
#define FULLBOARD (BOARDMAX-1)
#define POSSUEM_VIZINHO_ESQUERDA 279258638311359LL
#define POSSUEM_VIZINHO_DIREITA 558517276622718LL

#endif
