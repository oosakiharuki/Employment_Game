#include "MyGame.h"

//Windowsアプリのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<EngineLayer::Framework> game = std::make_unique<MyGame>();

	game->Run();

	game.reset();
	return 0;
}