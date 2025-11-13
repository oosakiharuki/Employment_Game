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
	void SetNextStage(std::string fileName) override { fileName_ = fileName; }

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

	std::string fileName_;

	//小さくする
	float t = 0.0f;
	//少しだけ大きくして緩急をつける
	const float kLittleLarge = -1.5f;
};

