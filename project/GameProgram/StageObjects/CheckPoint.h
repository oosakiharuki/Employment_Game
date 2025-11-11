#pragma once
#include "IStageObject.h"

/// <summary>
/// チェックポイント(IStageObjectの派生クラス)
/// </summary>
class CheckPoint : public IStageObject {
public:
	~CheckPoint() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void SetObjectName()override;

private:
};

