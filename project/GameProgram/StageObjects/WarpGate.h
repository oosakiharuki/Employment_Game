#pragma once
#include "IStageObject.h"

class WarpGate : public IStageObject
{
public:
	~WarpGate() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void SetNextStage(std::string fileName) { fileName_ = fileName; }
	std::string GetNextStage() { return fileName_; }

	/// <summary>
	/// ゲートを小さくして消す
	/// </summary>
	void Vanish();
private:

	std::string fileName_;

	//小さくする
	float t = 0.0f;
	//少しだけ大きくして緩急をつける
	const float littleLarge = -1.5f;
};

