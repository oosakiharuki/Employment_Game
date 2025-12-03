#pragma once
#include "Object_glTF.h"
#include "MyMath.h"
#include "GameActor.h"

/// <summary>
/// 傘(発泡、守が使える)
/// </summary>
class Umbrella : public GameActor{
public:
	~Umbrella();
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// getter_座標
	/// </summary>
	/// <returns>現在の座標</returns>
	Vector3 GetTranslate() { return wt_.translation_; }
	/// <summary>
	/// setter‗座標
	/// </summary>
	/// <param name="translate">代入する座標</param>
	void SetTranslate(Vector3 translate) { wt_.translation_ = translate; }

	/// <summary>
	/// setter‗回転
	/// </summary>
	/// <param name="rotate">代入する各回転角度</param>
	void SetRotate(Vector3 rotate) { wt_.rotation_ = rotate; }

	/// <summary>
	/// setter‗スケール
	/// </summary>
	/// <param name="scale">代入するスケール</param>
	void SetScale(Vector3 scale) { wt_.scale_ = scale; }

	/// <summary>
	/// シールドモード
	/// </summary>
	/// <param name="isShield">trueはシールドモードに変更</param>
	void ShieldMode(bool isShield) { isShieldMode_ = isShield; }

private:
	//オブジェクト設定
	std::unique_ptr<Object_glTF> object_;
	//AABBのサイズ
	const Vector3 kAABBSize_ = { 1,2,1 };
	//シールドモード
	bool isShieldMode_ = false;
};

