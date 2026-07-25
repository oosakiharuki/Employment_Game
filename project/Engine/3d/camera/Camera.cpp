/// ---------
/// 
/// カメラ
/// 
/// ---------
#include "Camera.h"
#include "WinApp.h"
using namespace MyMath;

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {

	Camera::Camera() {
		worldTransform_.Initialize();//ワールド座標更新
		//Transform更新処理
		transform_ = worldTransform_.UpdateTransform();
		aspect_ = float(WinApp::kClientWidth_) / float(WinApp::kClientHeight_);//アスペクト比(windowsの幅/高さ)
		worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);//ワールド座標行列
		viewMatrix_ = Inverse(worldMatrix_);//ビュー行列
		projectionMatrix_ = MakePerspectiveFovMatrix(forY_, aspect_, nearClip_, farClip_);//
		viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);//ビュープロジェクション行列
	}

	void Camera::Update() {
		worldTransform_.UpdateMatrix(transform_);
		worldMatrix_ = worldTransform_.GetMatWorld();
		viewMatrix_ = Inverse(worldMatrix_);
		projectionMatrix_ = MakePerspectiveFovMatrix(forY_, aspect_, nearClip_, farClip_);
		viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
	}
}