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
	/// <returns>インスタンス</returns>
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

	/// <summary>
	/// setter_スプライトのテクスチャ
	/// </summary>
	/// <param name="name">コンテナの名前</param>
	/// <param name="texturePath">テクスチャパス</param>
	void SetSpriteTexture(const std::string name, const std::string& texturePath);
	/// <summary>
	/// getter_スプライトのテクスチャ
	/// </summary>
	/// <param name="name">コンテナの名前</param>
	/// <returns>テクスチャのファイルパス</returns>
	std::string GetSpriteTexture(const std::string name);

private:
	//インスタン
	static std::unique_ptr<UIManager> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<UIManager>;
	//スプライトのコンテナ
	std::unordered_map<std::string, std::unique_ptr<Sprite>> sprites_;

	/// <summary>
	/// スプライトの初期設定
	/// </summary>
	/// <param name="sprite">スプライトクラス</param>
	/// <param name="spriteData">スプライトデータ</param>
	void InitSprite(std::unique_ptr<Sprite>& sprite, const SpriteData& spriteData);
};

