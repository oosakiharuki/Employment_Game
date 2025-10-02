#pragma once
#include "IStageObject.h"

class CheckPoint : public IStageObject {
public:
	~CheckPoint() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
};

