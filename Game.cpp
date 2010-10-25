#include <irrlicht.h>

#include "Game.h"
#include "bit.h"
#include "ia.h"

using namespace irr;
using namespace gui;
#pragma comment(lib, "Irrlicht.lib")

const wchar_t* GAME_TITLE = L"GO!";

#define MENU_NEW_GAME 300
#define MENU_OPTIONS 301
#define MENU_EXIT 302
#define MENU_ABOUT 303

#define OPTIONS_WINDOW 665
#define OPTIONS_COMBO_BOX 666
#define OPTIONS_OK 667
#define OPTIONS_COMBO_PH_PC 0
#define OPTIONS_COMBO_PH_BC 1
#define OPTIONS_COMBO_BH_PC 2
#define OPTIONS_COMBO_BH_BC 3

////////////////////////////////////////////////
//  E V E N T   R E C E I V E R               //
////////////////////////////////////////////////
EventReceiver::EventReceiver(Game* game){
	game_ = game;
}
bool EventReceiver::OnEvent(const SEvent& event){
	bool guiEvent = event.EventType == EET_GUI_EVENT;
	if (guiEvent){
		IGUIContextMenu* menu ;
		s32 id;

		switch(event.GUIEvent.EventType){
			case EGET_MENU_ITEM_SELECTED:	
			menu = (IGUIContextMenu*)event.GUIEvent.Caller;
			id = menu->getItemCommandId(menu->getSelectedItem());	

				switch(id){
					case MENU_NEW_GAME: game_->beginGame();
										game_->closeMenuOptions();
						break;
					case MENU_OPTIONS: game_->showMenuOptions();
						break;
					case MENU_EXIT: game_->endGame();
						break;
					case MENU_ABOUT: game_->showMenuAbout();
						break;

					default: return false;
						break;
				}
				break;
			case EGET_BUTTON_CLICKED:
				// único botão: ok das opções
				game_->setOptions();
				game_->closeMenuOptions();
				game_->beginGame();
				break;
		}
		return false;
	}

	bool mouseEvent = event.EventType == EET_MOUSE_INPUT_EVENT;
	bool lMouseLeftUp = event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP;
	
	if ( mouseEvent && lMouseLeftUp){
		return game_->mouseEvent(int(event.MouseInput.X), int(event.MouseInput.Y));
	}
	return false;
}
////////////////////////////////////////////////
//  G A M E                                   //
////////////////////////////////////////////////
Game::Game(){
	//config
	firstPlayer_  = BLACK;
	humanPlayer_  = BLACK;
	boarderActive_ = true;

	//initialize bit-counter
	startBitCounter();
	inicializaCache();

	beginGame();
	startDevice();

	//load images
	background_ = driver_->getTexture("tabu1.png");
	stones_ = driver_->getTexture("pecas2.png");
	
}
Game::~Game(){
	device_->drop();
}

void Game::update(){
	driver_->beginScene(true, true, video::SColor(0,0,0,0));
	drawStuff();
	environment_->drawAll();
	driver_->endScene();
}


void Game::drawStuff(){
	//draw background
	driver_->draw2DImage(background_, core::position2d<s32>(0,0));
	//draw whoIsPLaying
	if(humanPlayer_ == WHITE)
		driver_->draw2DImage(stones_, core::position2d<s32>(662,95),core::rect<s32>(60,0,120,60),0,video::SColor(255,255,255,255),true);
	else
		driver_->draw2DImage(stones_, core::position2d<s32>(662,95),core::rect<s32>(0,0,60,60),0,video::SColor(255,255,255,255),true);
	//draw stones
	for(int i = 0; i < 7; i++)
		for(int j = 0; j < 7; j++)
			if( i == lastY_ && j == lastX_ ){
				if( getBit(white_board_,j,i))
					driver_->draw2DImage(stones_, core::position2d<s32>((j*83) + 34,(i*83) + 36),core::rect<s32>(60,0,120,60),0,video::SColor(255,255,100,100),true);
				else if( getBit(black_board_,j,i))
					driver_->draw2DImage(stones_, core::position2d<s32>((j*83) + 34,(i*83) + 36),core::rect<s32>(0,0,60,60),0,video::SColor(255,255,100,100),true);				
			}else{
				if( getBit(white_board_,j,i))
					driver_->draw2DImage(stones_, core::position2d<s32>((j*83) + 34,(i*83) + 36),core::rect<s32>(60,0,120,60),0,video::SColor(255,255,255,255),true);
				else if( getBit(black_board_,j,i))
					driver_->draw2DImage(stones_, core::position2d<s32>((j*83) + 34,(i*83) + 36),core::rect<s32>(0,0,60,60),0,video::SColor(255,255,255,255),true);				
			}
}
void Game::endGame(){
	gameOver_ = true;
	//std::cout << "velha";
}
void Game::beginGame(){
	gameOver_ = false;
	whoIsPlaying_ = firstPlayer_;

	//inicializa tabuleiro
	white_board_ = 0;
	black_board_ = 0;
	
	inicializaCache();

	//
	if( firstPlayer_ != humanPlayer_ ){
		AIplay();
	}
}
bool Game::gameOver() const{
	return !(device_->run()) || gameOver_;
}
bool Game::isBoarderActive(){
	return boarderActive_ || ! isMenuOptionActive();
}

void Game::showMenuAbout(){
	//IGUIWindow* about = 
		environment_->addMessageBox(L"Sobre", L"Esse jogo foi feito por puras razoes didaticas, e proibido vender, comercializar ou jogar esse jogo \nsem autorizacao previa dos criadores e do Reitor da Universidade Federeal do Rio Grande do Sul");	
}
void Game::showMenuOptions(){
	boarderActive_ = false;
	
	//Se a janela está aberta sai
	if (isMenuOptionActive()) return;

	IGUIWindow* window = environment_->addWindow(core::rect<s32>(30,30,270,130), false, L"Opcoes", 0, OPTIONS_WINDOW);	
	IGUIComboBox* cb = environment_->addComboBox(core::rect<s32>(30,30,210,60), window, OPTIONS_COMBO_BOX);
	cb->addItem(L"Preto Humano, Preto Comeca");
	cb->addItem(L"Preto Humano, Branco Comeca");
	cb->addItem(L"Branco Humano, Preto Comeca");
	cb->addItem(L"Branco Humano, Branco Comeca");
	environment_->addButton(core::rect<s32>(30,60,210,90), window, OPTIONS_OK, L"Aplicar", L"Ao aplicar as novas configuracoes o jogo vai recomecar");
}
bool Game::isMenuOptionActive(){
	IGUIElement* root = environment_->getRootGUIElement();
	IGUIElement* e = root->getElementFromId(OPTIONS_WINDOW, true);
	return !( e == NULL);
}
void Game::setOptions(){
	IGUIElement* root = environment_->getRootGUIElement();
	IGUIComboBox* cb = (IGUIComboBox*) root->getElementFromId(OPTIONS_COMBO_BOX, true);

	switch(cb->getSelected()){
	case OPTIONS_COMBO_PH_PC :
			firstPlayer_  = BLACK;
			humanPlayer_  = BLACK;
		break;
	case OPTIONS_COMBO_PH_BC :
			firstPlayer_  = WHITE;
			humanPlayer_  = BLACK;
		break;
	case OPTIONS_COMBO_BH_PC :
			firstPlayer_  = BLACK;
			humanPlayer_  = WHITE;
		break;
	case OPTIONS_COMBO_BH_BC :
			firstPlayer_  = WHITE;
			humanPlayer_  = WHITE;
		break;
	}
}
void Game::closeMenuOptions(){
	// procura a janela opcões e fecha
	IGUIElement* root = environment_->getRootGUIElement();
	IGUIElement* e = root->getElementFromId(OPTIONS_WINDOW, true);
	if (e) e->remove();
}
void Game::startDevice(){
	device_ = createDevice(video::EDT_SOFTWARE,core::dimension2d<s32>(800, 600));
	driver_ = device_->getVideoDriver();

	receiver_ = new EventReceiver(this);
	device_->setEventReceiver(receiver_);

	environment_ = device_->getGUIEnvironment();
	
	//GUI
	//
	IGUISkin* skin = environment_->getSkin();
	IGUIFont* font = environment_->getFont("fonthaettenschweiler.bmp");
	if (font)
		skin->setFont(font);

	IGUIContextMenu* menu = environment_->addMenu();
	menu->addItem(L"Jogo", -1, true, true);
	menu->addItem(L"Ajuda", -1, true, true);

	IGUIContextMenu* submenu;
	submenu = menu->getSubMenu(0);
	submenu->addItem(L"Novo Jogo", MENU_NEW_GAME);
	submenu->addItem(L"Opcoes", MENU_OPTIONS );
	submenu->addSeparator();
	submenu->addItem(L"Sair", MENU_EXIT);

	submenu = menu->getSubMenu(1);
	submenu->addItem(L"Sobre", MENU_ABOUT);

	for (s32 i=0; i<gui::EGDC_COUNT ; ++i){
		video::SColor col = environment_->getSkin()->getColor((gui::EGUI_DEFAULT_COLOR)i);
        col.setAlpha(255);
        environment_->getSkin()->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
    }
	device_->setWindowCaption(GAME_TITLE); 	
}
#define CORRECTION 20
#define GRID_SIZE 83
bool Game::mouseEvent(int x, int y){
	if( ! isBoarderActive() )
		return false;

	x += CORRECTION;
	y += CORRECTION;
	// -1 == posição ilegal
	int posX = -1, posY = -1;
	
	if ( (x % GRID_SIZE) < CORRECTION )
		posX = (x - GRID_SIZE) / GRID_SIZE; 
	else if( (x % GRID_SIZE) > GRID_SIZE - CORRECTION )
		posX = ((x - GRID_SIZE) / GRID_SIZE) + (( x < GRID_SIZE) ? 0 : 1 ); 
	
	if ( (y % GRID_SIZE) < CORRECTION )
		posY = (y - GRID_SIZE) / GRID_SIZE; 
	else if( (y % GRID_SIZE) > GRID_SIZE - CORRECTION )
		posY = ((y - GRID_SIZE) / GRID_SIZE) + (( y < GRID_SIZE) ? 0 : 1 ); 
	
	bool legalPosition = ( posX >= 0 ) && ( posY >= 0 ) && ( posX < 7 ) && ( posY < 7 ) ;
	bool noPieceHere   = 0 == (getBit(white_board_,posX,posY) | getBit(black_board_,posX,posY));
	if ( legalPosition && noPieceHere){
		
		humanPlay(posX,posY);

		/*
		Board temp1,temp2;
		territorios(black_board,white_board,temp1,temp2);
		int ganhou = victoryTestBlackWhite(black_board,white_board,whoIsPlaying_);
		if (ganhou == BLACK) {
			black_board = FULLBOARD;
			white_board = 0;
		}
		else if (ganhou == WHITE) {
			black_board = 0;
			white_board = FULLBOARD;
		}
		whoIsPlaying_ = ( whoIsPlaying_ == WHITE ) ? BLACK : WHITE;
		*/
		return true;
	}
	else
		return false;
}

void Game::AIplay(){
	Board move;
	int dificuldade = 0;
	if (humanPlayer_ == BLACK){
			move = AIchooseMovement(white_board_, black_board_,dificuldade);
			u64ToXY(move,lastX_, lastY_);
			white_board_ = white_board_ | move;
	}else{
			move = AIchooseMovement(black_board_, white_board_,dificuldade);
			u64ToXY(move,lastX_, lastY_);			
			black_board_ = black_board_ | move;
	}
}

char Game::aiPlayer() {
	if (humanPlayer_ == BLACK)
		return WHITE;
	else
		return BLACK;
}

void Game::humanPlay(int x, int y){
	if (victoryTestBlackWhite(black_board_,white_board_,aiPlayer()) == EMPTY) {
		if (humanPlayer_ == BLACK)
			black_board_ = setBit(black_board_,x,y);
		else
			white_board_ = setBit(white_board_,x,y);
		update();
		int ganhador = victoryTestBlackWhite(black_board_,white_board_,humanPlayer_);
		if (ganhador == EMPTY) {
			AIplay();
			ganhador = victoryTestBlackWhite(black_board_,white_board_,aiPlayer());
			if (ganhador != EMPTY)
				vitoria(ganhador);
		}
		else
			vitoria(ganhador);
	}
}

 void Game::vitoria(int jogador){
	if( jogador == humanPlayer_ )
		environment_->addMessageBox(L"Vitoria!", L"Voce(jogador humano) ganhou, parabens!");	
	else
		environment_->addMessageBox(L"Derrota!", L"Voce(jogador humano) perdeu, eu sabia que voce ia perder!!");
}