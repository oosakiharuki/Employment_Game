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
/// 操作ガイド
/// </summary>
class UIManager
{
public:

	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static UIManager& GetInstance();

	void CreateSprite(const SpriteData& spriteData);

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

	void SetPlayerTranslate(const Vector3& translate) { playerTranslate_ = translate; }

	void SetSpriteTexture(const std::string name, const std::string& texturePath);

	std::string GetSpriteTexture(const std::string name);

private:
	//インスタン
	static std::unique_ptr<UIManager> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<UIManager>;

	std::unordered_map<std::string, std::unique_ptr<Sprite>> sprites_;
	std::unordered_map<std::string, std::unique_ptr<Sprite>> spriteGuides_;

	Vector3 playerTranslate_;

	/// <summary>
	/// スプライトの初期設定
	/// </summary>
	void InitSprite(std::unique_ptr<Sprite>& sprite, const SpriteData& spriteData);
};

