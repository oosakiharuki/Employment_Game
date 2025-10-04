#pragma once
#include "Object_glTF.h"
#include "MyMath.h"

class Umbrella {
public:
	~Umbrella();
	void Initialize();
	void Update();
	void Draw();

	Vector3 GetTranslate() { return wt.translation_; }
	void SetTranslate(Vector3 translate) { wt.translation_ = translate; }

	void SetRotate(Vector3 rotate) { wt.rotation_ = rotate; }

	void SetScale(Vector3 scale) { 
		wt.scale_ = scale;
		scaleTimer = 0.0f;
	}


	AABB GetAABB();

	void ShieldMode(bool isShield) { isShieldMode = isShield; }

	void ScaleUpdate(bool* mosionOn, Vector3 scale, const float maxTime);
private:
	std::unique_ptr<Object_glTF> object;

	WorldTransform wt;
	AABB umbrellaAABB;

	bool isShieldMode = false;

	const float deltaTime = 1.0f / 60.0f;

	bool isDamageMosion = false;
	Vector3 damageScale = { 0.1f, 0.1f, 0.1f };
	const float damageMaxTime = 0.025f;

	float scaleTimer = 0.0f;
	bool TimeReturn = false;
};

