#include "PlayerBullet.h"

using namespace MyMath;

PlayerBullet::~PlayerBullet() {}


void PlayerBullet::Initialize() {
	object = std::make_unique<Object3d>();
	object->Initialize();
	object->SetModelFile("PlayerBullet.obj");

	wt.Initialize();

	bulletAABB.min = { -1,-1,-1 };
	bulletAABB.max = { 1,1,1 };
}

void PlayerBullet::Update() {

	deathTimer += deltaTime;
	
	//弾丸速度
	//徐々に減速する
	wt.translation_ += EaseOut(velocity_,deathTimer,endTime);

	//時間がたったら消える
	if (deathTimer >= endTime) {
		isDead = true;
	}

	object->Update(wt);
	wt.UpdateMatrix();
}

void PlayerBullet::Draw() {
	object->Draw();
}

AABB PlayerBullet::GetAABB() {
	AABB aabb;
	aabb.min = wt.translation_ + bulletAABB.min;
	aabb.max = wt.translation_ + bulletAABB.max;
	return aabb;
}
