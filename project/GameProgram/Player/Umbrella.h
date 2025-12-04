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

