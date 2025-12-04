#pragma once 
#include "Framework.h"
#include "SceneManager.h"
#include "FadeScreen.h"
/// <summary>
/// 自作ゲーム
/// </summary>
class MyGame : public Framework{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

private:
	std::unique_ptr<SceneManager> sceneManager_ = nullptr;
	FadeScreen* fadeScreen_ = nullptr;
};