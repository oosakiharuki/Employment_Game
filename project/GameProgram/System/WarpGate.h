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
private:

	std::string fileName_;

};

