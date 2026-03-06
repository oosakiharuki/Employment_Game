#pragma once
#include "IStageObject.h"

/// <summary>
/// ワープゲート(IStageObjectの派生クラス)
/// </summary>
class WarpGate : public IStageObject
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~WarpGate() override;
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;
	/// <summary>
	/// setter_次のステージ
	/// </summary>
	/// <param name="fileName"></param>ステージ名(例:stage_01)
	void SetNextStage(const std::string& fileName) override { fileName_ = fileName; }

	/// <summary>
	/// getter_次のステージ
	/// </summary>
	/// <returns></returns>次のステージ
	std::string GetNextStage() override { return fileName_; }

	/// <summary>
	/// ゲートを小さくして消す
	/// </summary>
	void Vanish();

	/// <summary>
	/// ワープの出口モード
	/// </summary>
	/// <param name="translate">登場するプレイヤーの位置</param>
	void WarpExit(const Vector3& translate);

private:
	/// <summary>
	/// 当たり判定コマンド
	/// </summary>
	/// <param name="collision">相手側の当たり判定ソース</param>
	void OnCollision(CollisionSource* collision) override;

	/// <summary>
	/// プレイヤーに触れたワープゲート処理
	/// </summary>
	void TouchWarpGate();

	bool scaleFlag_ = false;
	float scaleTimer_ = 0.0f;

	//次のステージの名前
	std::string fileName_;

	//小さくする
	float smallingTimer_ = 0.0f;

	float largeTimer_ = 0.0f;
	const Vector3 kLargeMax_ = { 1.5f, 1.5f, 1.5f };

	bool largeFlag_ = true;

	bool warpExitMode_ = false;
	float minUnder_ = 1000.0f;

	Vector3 position_;

	const float kLittleUp_ = 0.01f;
};

