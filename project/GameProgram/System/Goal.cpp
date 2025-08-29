#include "Goal.h"

using namespace MyMath;

Goal::~Goal() {
	delete object_;
}

void Goal::Initialize(){
	wt.Initialize();

	object_ = new Object3d();
	object_->Initialize();
	object_->SetModelFile("goal.obj");

}
	
void Goal::Update(){
	wt.UpdateMatrix();
}
	
void Goal::Draw(){
	object_->Draw(wt);
}

AABB Goal::GetAABB() {
	AABB aabb;
	aabb.min = wt.translation_ + goalAABB.min;
	aabb.max = wt.translation_ + goalAABB.max;
	return aabb;
}
