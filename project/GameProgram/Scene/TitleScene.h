#pragma once
#include "Sprite.h"
#include "Framework.h"
#include "IScene.h"

/// <summary>
/// タイトルシーン(ISceneの派生クラス)
/// </summary>
class TitleScene : public IScene{
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:

	WorldTransform wts[4];

	//タイトルで使うオブジェクト
	std::unique_ptr<Object_glTF> object_autoPlayer = nullptr;
	std::unique_ptr<Object_glTF> umbrella;//傘+矢印
	std::unique_ptr<Object_glTF> object_Moji_Start;//スタートする
	std::unique_ptr<Object_glTF> object_Moji_End;//ゲーム終了する
	std::unique_ptr<Object_glTF> object_stage;//タイトル用のステージ

	std::unique_ptr<Shadow> player_shadow;//プレイヤー影

	std::unique_ptr<Sprite> sprite_Moji_Title;//タイトル名

	std::unique_ptr<Particle> bullet_particle;//決定した時にパーティクルを出す

	bool isbullet = false;

	//時間
	float timer = 0.0f;
	const float kTimeMax = 1.0f;
	
	//上下に移動する
	float move = 0.0f;
	Vector3 start = {-2.0f,-100.0f,0.0f};
	Vector3 end = { -2.0f,32.0f,0.0f};

	float bulletTimer = 0.0f;
	const float kBulletTimeMax = 1.0f;

	Vector2 Title_pos = { 430,-300 };

	//通常プレイと同じ重力
	float gravity = 0.05f;

	//最初、文字をふせておく
	float rotate_select_moji = 180.0f;
};