#include "bit.h"

char bits_in_16bits [0x1u << 16] ;     
int itebitcount (unsigned int n) {
   int count = 0;
   while (n) {
      count += n & 0x1u;
      n >>= 1;
   }
   return count;
}

void startBitCounter() {
     for (unsigned int i = 0; i < 0x1u << 16; i++)
         bits_in_16bits[i] = itebitcount(i);
}
 

char bitcount (Board n)  {
   return bits_in_16bits [n         & 0xffffu]
       +  bits_in_16bits [(n >> 16) & 0xffffu]
       +  bits_in_16bits [(n >> 32) & 0xffffu]
       +  bits_in_16bits [(n >> 48)];
}

/*void debugBoard(Board n) {
	for (int y = 0; y < 7; y++) {
		for (int x = 0; x < 7; x++)
			if (getBit(n,x,y))
				std::cout<<1;
			else
				std::cout<<0;
		std::cout<<std::endl;
	}
	std::cout<<std::endl;
}*/
void u64ToXY(Board board, int &x, int &y) {
 x = bitcount(board-1) % 7;
 y = bitcount(board-1) / 7;
}
