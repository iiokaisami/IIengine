#include "gameEngine/baseScene/MyGame.h"

#include <memory> 

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	std::unique_ptr<Framework> game = std::make_unique<MyGame>();

	game->Run();

	return 0;
}