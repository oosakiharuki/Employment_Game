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
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static SceneManager& GetInstance();


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
	//インスタンス
	static std::unique_ptr<SceneManager> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<SceneManager>;

	std::unique_ptr<BaseScene> scene_ = nullptr;//現在シーン
	std::unique_ptr<BaseScene> nextScene_ = nullptr;//次のシーン

	AbstractSceneFactory* sceneFactory_ = nullptr;

};