#include "Guide.h"
#include "Input.h"

void Guide::Initialize(const Transform& transform, const std::string& textureFileName) {
	object_ = std::make_unique<Object3d>();
	object_->Initialize();
	object_->SetModelFile("Guide.obj");

	wt_.Initialize();

	transform_ = transform;

	textureFileName_ = textureFileName;
}

void Guide::Update() {
	//最初が[G]でない ステージ番号に使える
	if (textureFileName_.c_str()[0] != 'G') {
		textureFilePath_ = textureFileName_;
	}
	else if (Input::GetInstance().GetJoystickState()) {
		textureFilePath_ = textureFileName_ + "GamePad";
	}
	else {
		textureFilePath_ = textureFileName_ + "KeyBoard";
	}

	wt_.UpdateMatrix(transform_);
	object_->Update(wt_);
}

void Guide::Draw() {
	object_->Draw(textureFilePath_);
}
