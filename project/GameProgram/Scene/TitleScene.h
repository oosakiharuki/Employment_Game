#pragma once
#include "Sprite.h"
#include "Framework.h"
#include "IScene.h"

class TitleScene : public IScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	Object_glTF* object_autoPlayer = nullptr;

	WorldTransform wts[4];

	std::unique_ptr<Object_glTF> umbrella;
	std::unique_ptr<Object_glTF> object_Moji_Start;
	std::unique_ptr<Object_glTF> object_Moji_End;
	std::unique_ptr<Object_glTF> object_stage;

	std::unique_ptr<Shadow> player_shadow;

	Sprite* sprite_Moji_Title;

	std::unique_ptr<Particle> bullet_particle;

	bool isbullet = false;

	float timer = 0.0f;
	const float TimeMax = 1.0f;
	float move = 0.0f;
	Vector3 start = {-2.0f,-100.0f,0.0f};
	Vector3 end = { -2.0f,32.0f,0.0f};

	float bulletTimer = 0.0f;
	const float bulletTimeMax = 1.0f;

	Vector2 Title_pos = { 430,-300 };


	float gravity = 0.05f;

	float rotate_select_moji = 180.0f;
};