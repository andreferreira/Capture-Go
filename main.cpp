#include "Game.h"

int main(){
	Game game;
	while( ! game.gameOver() ){
		game.update();
	}
	return 0;
}
