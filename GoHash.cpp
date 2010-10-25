#include "GoHash.h"

//template<class T>
GoHash::GoHash(int maxSize){
	maxSize_ = maxSize;
	elements_ = new  GoHashNode[maxSize][ASSOCIATIVITY];
	//for(int i = 0; i < maxSize_; ++i){
	//	elements_[i] = NULL;
	//	elements_[i] = NULL;
	//}
	//elements[2] = new Board[3];
	//elements[2][2] = 12;

	//std::cout << elements[2][2];
	//std::cin.get();
}

GoHash::~GoHash() {
	delete[] elements_;
}

unsigned char randomBitstrings[14][256];

void inicializaRandombits() {
	for (int linha = 0; linha < 14; linha++)
		for (int i = 0; i < 256; i++)
			randomBitstrings[linha][i] = rand();
}

int GoHash::key(Board max,Board min){
	int chavemax =  randomBitstrings[0][max & 0xff] ^
				    randomBitstrings[1][((max >> 8) & 0xff)] ^ 
					randomBitstrings[2][((max >> 16) & 0xff)] ^
					randomBitstrings[3][((max >> 24) & 0xff)] ^
                    randomBitstrings[4][((max >> 32) & 0xff)] ^
                    randomBitstrings[5][((max >> 40) & 0xff)] ^
                    randomBitstrings[6][((max >> 48) & 0xff)];

	int chavemin = randomBitstrings[7][(min & 0xff)] ^
				    randomBitstrings[8][((min >> 8) & 0xff)] ^ 
					randomBitstrings[9][((min >> 16) & 0xff)] ^
					randomBitstrings[10][((min >> 24) & 0xff)] ^
					randomBitstrings[11][((min >> 32) & 0xff)] ^
                    randomBitstrings[12][((min >> 40) & 0xff)] ^
                    randomBitstrings[13][((min >> 48) & 0xff)];

	int chave = (chavemax << 8) | chavemin;
	return chave % maxSize_ ;
}



void GoHash::insert(Board max,Board min,node value, int level){
	int chave = key(max,min);
	
	GoHashNode g1(max,min,value,level);
	/*if ( elements_[chave] == NULL ){
		elements_[chave] = new GoHashNode[3];
		//elements_[chave][0].set(max,min,value);
		elements_[chave][0] = g1;
	}else{*/

	for (int i = ASSOCIATIVITY-1; i > 0; i--)
		elements_[chave][i] = elements_[chave][i-1];
	elements_[chave][0] = g1;
	//}
}

int usos = 1;
int miss = 1;
int GoHash::get(Board max,Board min,int level){
	int chave = key(max,min);
	GoHashNode g1(max,min,0,level);
	/*if (usos % 5000 == 0) {
		std::cout<<"Usos"<<usos<<" Miss:"<<miss<<" Porcentagem:"<<(double)miss/usos<<std::endl;
		usos = 1;
		miss = 1;
	}
	usos++;*/

	for (int i = 0; i < ASSOCIATIVITY; ++i) {
		if ( elements_[chave][i] == g1 ) {
			return elements_[chave][i].data_;
		}
	}
    miss++;	
	return NOT_FOUND;
}
/*int main(){
	GoHash g1(12);
	g1.insert(66666,67666,15);
	g1.insert(16661,167661,16);

	std::cout << g1.get(66666,67661) << "\n";
	std::cout << g1.get(66666,67666) << "\n";
	g1.insert(66666,67666,13);
	std::cout << g1.get(66666,67666) << "\n";
	std::cout << g1.get(16661,167661) << "\n";
	std::cin.get();
	return 0;
}*/

