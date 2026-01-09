#include "MoveGround.h"

using namespace MyMath;

MoveGround::~MoveGround() {}


void MoveGround::Initialize() {
	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("moveGround.obj");
}

void MoveGround::Update() {

	MoveRoute();

	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);
}

void MoveGround::Draw() {
	object_->Draw();
}

void MoveGround::SetTravelRoute(const Vector3& pointA, const Vector3& pointB) {
	pointA_ = transform_.translate + pointA;
	pointB_ = transform_.translate + pointB;
}

void MoveGround::MoveRoute() {
	
	if (isGameStart_) {
		segment_.origin = transform_.translate;
		
		isGameStart_ = false;
	}

	//目的地の設定 pointSwitchが(true: pointA_が目的地 / false: pointB_が目的地)
	(pointSwitch_) ? segment_.diff = pointA_ : segment_.diff = pointB_;

	//目的地まで移動
	transform_.translate += GoDestination(segment_) / moveFrame_;//moveFrame_で距離を分割

	//目的地についたとき
	if (GoDestination(transform_.translate, segment_.diff) <= Vector3{ 0.1f,0.1f,0.1f } &&
		GoDestination(transform_.translate, segment_.diff) >= Vector3{ -0.1f,-0.1f,-0.1f }) {
		pointSwitch_ = !pointSwitch_;//移動スイッチの切り替え
		segment_.origin = transform_.translate;
	}
}
