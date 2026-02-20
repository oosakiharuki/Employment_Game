#pragma once
#include "IStageObject.h"

/// <summary>
/// ワープゲート(IStageObjectの派生クラス)
/// </summary>
class WarpGate : public IStageObject
{
public:
	~WarpGate() override;
	void Initialize() override;
	void Update() override;
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
private:
	
	void OnCollision(CollisionSource* collision) override;
	bool a = false;
	bool scaleFlag_ = false;
	float scaleTimer_ = 0.0f;

	//次のステージの名前
	std::string fileName_;

	//小さくする
	float smallingTimer_ = 0.0f;

	float largeTimer_ = 0.0f;
	const Vector3 kLargeMax_ = { 1.5f, 1.5f, 1.5f };

	bool largeFlag_ = true;
};

