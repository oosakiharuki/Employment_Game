#pragma once
#include "IStageObject.h"

class Goal : public IStageObject {
public:

	~Goal() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
};