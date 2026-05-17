#pragma once
#include "MyMath.h"
#include "Object_gltf.h"

/// <summary>
/// 見た目専用オブジェクト
/// (PlayerのモデルだがPlayer処理を行わない、Titleなどで使う)
/// </summary>
class VisualActor {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	VisualActor();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~VisualActor();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="objectName">オブジェクトの名前</param>
	void Initialize(const std::string objectName);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
	/// <summary>
	/// setter_トランスフォーム
	/// </summary>
	/// <param name="transform">トランスフォームを設定</param>
	void SetTransform(const Transform& transform) { transform_ = transform; }
	/// <summary>
	/// getter_トランスフォーム
	/// </summary>
	/// <returns>現在のトランスフォーム</returns>
	const Transform& GetTransform() { return transform_; }
	/// <summary>
	/// setter_座標位置
	/// </summary>
	/// <param name="translate"></param>
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }
	/// <summary>
	/// getter_座標位置
	/// </summary>
	/// <returns></returns>
	const Vector3& GetTranslate() { return transform_.translate; }
	/// <summary>
	/// getter_オブジェクトの名前
	/// </summary>
	/// <returns></returns>
	const std::string& GetObjectName() { return objectName_; }
	/// <summary>
	/// オブジェクトの変更
	/// </summary>
	/// <param name="changeObjectName">変更するオブジェクト名</param>
	void ChangeObject(const std::string& changeObjectName) { baseObject_->ChangeAnimation(changeObjectName); }
	/// <summary>
	/// ライトモード
	/// </summary>
	void LightOn();

private:
	EngineLayer::WorldTransform wt_;
	Transform transform_;

	std::unique_ptr<EngineLayer::Object_glTF> baseObject_;

	std::string objectName_;
};

