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

	SoundData soundData_;

	//残機
	uint32_t RemainingLife_ = 2;

	//スタート演出(水たまりから飛び出る感じに)
	bool isStartStage_ = true;
	Vector3 playerPoint_{};
	float startPointY_ = 0.0f;

	//スタート時のワープゲート
	std::unique_ptr<WarpGate> startWarp_;


	/// <summary>
	/// 水たまりのようなワープゲート出口
	/// </summary>
	void WarterWarpExit();

	/// <summary>
	/// リスポーン
	/// </summary>
	void Respawn();

};