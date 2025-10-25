#pragma once

#include <memory>
#include "IScene.h"
#include "GameScene.h"//geme
#include "TitleScene.h"//title
#include "SelectScene.h"
#include "ClearScene.h"
#include "GameOverScene.h"

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

	bool SetGameEnd() { return sceneArr_[currentSceneNo_]->GetIsGameEnd(); }

private:
	
	void SceneChange(int prev,int current);//シーン入れ替え

	//ステージの最大数
	static const uint32_t SceneNum = 5;
	
	std::unique_ptr<IScene> sceneArr_[SceneNum];

	int currentSceneNo_;//現在シーン
	int prevSceneNo_;//前シーン

};