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
	//最初が[Guide]で始まらない コントローラー、キーボードで変更するやつでないとき
	if (textureFileName_.substr(0,5) != "Guide") {
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
