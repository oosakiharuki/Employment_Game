#pragma once
#include "IStageObject.h"

/// <summary>
/// ゴール(IStageObjectの派生クラス)
/// </summary>
class Goal : public IStageObject {
public:

	~Goal() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
};