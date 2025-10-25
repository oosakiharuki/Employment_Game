#pragma once

#include <memory>
#include "IScene.h"
#include "GameScene.h"//geme
#include "TitleScene.h"//title
#include "SelectScene.h"
#include "ClearScene.h"
#include "GameOverScene.h"

#include "SceneFactory.h"

/// <summary>
/// シーンマネージャ
/// </summary>
class SceneManager {
public:
	SceneManager();
	~SceneManager();

	void Initialize();
	void Update();
	void Draw();
	void Finalize();

	bool SetGameEnd() { return sceneArr.iscene->GetIsGameEnd(); }

private:
	
	void SceneChange();//シーン入れ替え

	SceneGroup sceneArr;
	SceneFactory sceneFactory;

	std::string currentSceneNo_;//現在シーン
	std::string prevSceneNo_;//前シーン

};