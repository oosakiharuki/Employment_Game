#pragma once
#include "Object3d.h"
#include "MyMath.h"

class WarpGate
{
public:
	~WarpGate();
	void Initialize();
	void Update();
	void Draw();

	Vector3 GetPosition() { return wt.translation_; }
	void SetPosition(Vector3 position) { wt.translation_ = position; }

	AABB GetAABB();
	void SetAABB(AABB aabb) { warpGateAABB = aabb; }
	
	void SetNextStage(std::string fileName) { fileName_ = fileName; }
	std::string GetNextStage() { return fileName_; }
private:

	Object3d* object_ = nullptr;
	WorldTransform wt;
	AABB warpGateAABB;

	std::string fileName_;

};

