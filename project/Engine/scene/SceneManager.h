#pragma once

#include <memory>
#include "BaseScene.h"
#include "GameScene.h"//geme
#include "TitleScene.h"//title
#include "SelectScene.h"
#include "ClearScene.h"
#include "GameOverScene.h"

#include "AbstractSceneFactory.h"

/// <summary>
/// シーンマネージャ
/// </summary>
class SceneManager {
public:

	static std::shared_ptr<SceneManager> GetInstance();


	SceneManager();
	~SceneManager();

	/// <summary>
	/// 更新処理
	/// </summary>
	void SceneUpdate();
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
	bool SetGameEnd() { return scene_->GetIsGameEnd(); }

	/// <summary>
	/// 次のシーンに移りますよのフラグ
	/// </summary>
	bool NextSceneChangeFlag();

	/// <summary>
	/// シーン入れ替え
	/// </summary>
	void ChangeScene(const std::string& sceneName);

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private:

	static std::shared_ptr<SceneManager> sInstance_;

	//friend struct std::default_delete<SceneManager>;

	BaseScene* scene_ = nullptr;//現在シーン
	BaseScene* nextScene_ = nullptr;//次のシーン

	std::unique_ptr<BaseScene> settingScene_;

	AbstractSceneFactory* sceneFactory_ = nullptr;

};