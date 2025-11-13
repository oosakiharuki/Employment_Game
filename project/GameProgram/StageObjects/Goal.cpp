#include "Goal.h"

using namespace MyMath;

Goal::~Goal() {}

void Goal::Initialize(){
	wt.Initialize();

	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("goal.obj");

}
	
void Goal::Update(){
	object_->Update(wt);
	wt.UpdateMatrix();
}
	
void Goal::Draw(){
	object_->Draw();
}
