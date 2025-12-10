#include "SceneFactory.h"

#include <memory>

#include "../../application/scene/TitleScene.h"
#include "../../application/scene/GamePlayScene.h"
#include "../../application/scene/GameOverScene.h"
#include "../../application/scene/ClearScene.h"

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName)
{
	if (sceneName == "TITLE")
	{
		return std::make_unique<TitleScene>();
	}
	else if (sceneName == "GAMEPLAY")
	{
		return std::make_unique<GamePlayScene>();
	}
	else if (sceneName == "GAMEOVER")
	{
		return std::make_unique<GameOverScene>();
	}
	else if (sceneName == "CLEAR")
	{
		return std::make_unique<ClearScene>();
	}

	return nullptr;
}
