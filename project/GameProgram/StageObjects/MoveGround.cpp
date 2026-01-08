#include "MoveGround.h"

using namespace MyMath;

MoveGround::~MoveGround() {}


void MoveGround::Initialize() {
	wt_.Initialize();
	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("moveGround.obj");

}

void MoveGround::Update() {

	MoveRoute();

	object_->Update(wt_);
	wt_.UpdateMatrix();
}

void MoveGround::Draw() {
	object_->Draw();
}

void MoveGround::SetTravelRoute(const Vector3& pointA, const Vector3& pointB) {
	pointA_ = wt_.translation_ + pointA;
	pointB_ = wt_.translation_ + pointB;
}

void MoveGround::MoveRoute() {
	
	if (isGameStart_) {
		segment_.origin = wt_.translation_;
		
		isGameStart_ = false;
	}

	//目的地の設定 pointSwitchが(true: pointA_が目的地 / false: pointB_が目的地)
	(pointSwitch_) ? segment_.diff = pointA_ : segment_.diff = pointB_;


	wt_.translation_ += GoDestination(segment_) / moveFrame_;


	//目的地についたとき
	if (GoDestination(wt_.translation_, segment_.diff) <= Vector3{ 0.1f,0.1f,0.1f } &&
		GoDestination(wt_.translation_, segment_.diff) >= Vector3{ -0.1f,-0.1f,-0.1f }) {
		pointSwitch_ = !pointSwitch_;//移動スイッチの切り替え
		segment_.origin = wt_.translation_;
	}

}
