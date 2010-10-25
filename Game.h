#ifndef GAME_H
#define GAME_H

#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define MAX 1
#define MIN 2

#include <irrlicht.h>
#include <bitset>
#include "board.h"
using namespace irr;

class Game;

class EventReceiver :  public IEventReceiver
{
private:
	Game* game_;
public:
	EventReceiver(Game* game);
	bool OnEvent(const SEvent& event);
};


class Game
{
	EventReceiver* receiver_;
	gui::IGUIEnvironment* environment_;
	IrrlichtDevice*		 device_;
	video::IVideoDriver* driver_;

	video::ITexture* background_;
	video::ITexture* stones_;

	bool  gameOver_;
	Board white_board_;
	Board black_board_;
	char whoIsPlaying_;
	char firstPlayer_;
	char humanPlayer_;
	bool  boarderActive_; // apagar?

	int lastX_;
	int lastY_;

public:
		Game();
		~Game();
		bool gameOver() const;
		bool isBoarderActive();

		bool mouseEvent(int x, int y);
		void update();
		void endGame();
		void beginGame();

		void showMenuAbout();
		bool isMenuOptionActive();
		void showMenuOptions();
		void closeMenuOptions();
		void setOptions();
		//play!
		void AIplay();
		void humanPlay(int x,int y);
		char aiPlayer();
		void vitoria(int jogador);
private:
		void drawStuff();
		void startDevice();
};

#endif
