#include "gameEngine/baseScene/MyGame.h"

#include <memory> 

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	std::unique_ptr<IIEngine::Framework> game = std::make_unique<IIEngine::MyGame>();

	game->Run();

	return 0;
}