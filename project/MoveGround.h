#pragma once
#include "IStageObject.h"

class MoveGround : public IStageObject{
public:
	~MoveGround() override;
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void SetTravelRoute(const Vector3& pointA, const Vector3& pointB);

private:
	void MoveRoute();

	Vector3 pointA_{};
	Vector3 pointB_{};

	Segment segment_{};

	float moveFrame_ = 180.0f;

	//ゲームが開始して最初だけ通るフラグ
	bool isGameStart_ = true;

	bool pointSwitch_ = true;
};

