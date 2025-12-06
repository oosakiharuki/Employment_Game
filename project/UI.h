#pragma once
#include "MyMath.h"
#include "Sprite.h"
#include "Player.h"

struct SpriteData {
	std::string name;        //コンテナの名前
	std::string texturePath; //テクスチャ名
	Vector2 position;
	Vector2 size;
};

/// <summary>
/// 構造体_操作説明(ガイド)の設定
/// </summary>
struct Guide {
	SpriteData spriteData;  //スプライトデータ
	float lookPointX_left;  //見れる範囲_左端
	float lookPointX_right; //見れる範囲_右端
};

/// <summary>
/// 操作ガイド
/// </summary>
class UI
{
public:

	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static std::shared_ptr<UI> GetInstance();

	void CreateSprite(const SpriteData& spriteData);

	/// <summary>
	/// 操作説明スプライトを作る
	/// </summary>
	/// <param name="guide">名前や座標が入った構造体</param>
	void CreateGuide(const Guide& guide);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 描画処理
	/// プレイヤーの位置によって移す場所を変える
	/// </summary>
	void GuideDraw();

	void SetPlayerTranslate(const Vector3& translate) { playerTranslate_ = translate; }

	void SetSpriteTexture(const std::string name, const std::string& texturePath);

	std::string GetSpriteTexture(const std::string name);

private:
	static std::shared_ptr<UI> sInstance_;

	std::vector<Guide> guides_;
	
	std::unordered_map<std::string, std::unique_ptr<Sprite>> sprites_;
	std::unordered_map<std::string, std::unique_ptr<Sprite>> spriteGuides_;

	Vector3 playerTranslate_;

	/// <summary>
	/// スプライトの初期設定
	/// </summary>
	void InitSprite(std::unique_ptr<Sprite>& sprite, const SpriteData& spriteData);
};

