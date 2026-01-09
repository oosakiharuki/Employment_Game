#include "Goal.h"

using namespace MyMath;

Goal::~Goal() {}

void Goal::Initialize(){
	wt_.Initialize();
	//Transform更新処理
	transform_ = wt_.UpdateTransform();

	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("goal.obj");

}
	
void Goal::Update(){
	object_->Update(wt_);
	wt_.UpdateMatrix(transform_);
}
	
void Goal::Draw(){
	object_->Draw();
}
