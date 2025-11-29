#pragma once
#include "IScene.h"

/// <summary>
/// ゲームシーン(ISceneの派生クラス)
/// </summary>
class GameScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	//BGM
	SoundData soundData_;

	//スタート演出(水たまりから飛び出る感じに)
	bool isStartStage_ = true;
	Vector3 playerPoint_{};
	const float kStartWarpGateRange_ = 90.0f;

	float startPointY_ = -10.0f;
	//スタート時のワープゲート
	std::unique_ptr<WarpGate> startWarp_;
	//スタート時にプレイヤーを上げる変数
	const float kPlayerUp_ = 0.1f;
	//重ならないように影より上にする変数
	const float kWarpGateUpThanShadow_ = 0.02f;

	//落ちる限界
	const float kFallEndY_ = -10.0f;

	/// <summary>
	/// 水たまりのようなワープゲート出口
	/// </summary>
	void WarterWarpExit();

	/// <summary>
	/// リスポーン
	/// </summary>
	void Respawn();

};