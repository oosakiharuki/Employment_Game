#include "Goal.h"

using namespace MyMath;

Goal::~Goal() {}

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
