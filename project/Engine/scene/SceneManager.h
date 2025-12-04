#pragma once

#include <memory>
#include "BaseScene.h"
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

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
	/// <summary>
	/// 削除処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// ゲーム終了時の処理
	/// </summary>
	/// <returns></returns>trueで終了
	bool SetGameEnd() { return sceneArr_->GetIsGameEnd(); }

private:
	/// <summary>
	/// シーン入れ替え
	/// </summary>
	void SceneChange();

	/// <summary>
	/// シーンを作成し代入する
	/// </summary>
	void BuildScene();

	std::unique_ptr<BaseScene> sceneArr_;
	SceneFactory sceneFactory_;

	std::string currentScene_;//現在シーン
	std::string prevScene_;//前シーン

	std::unique_ptr<BaseScene> settingScene_;
};