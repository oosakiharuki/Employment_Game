#pragma once
#include "MyMath.h"
//ComPtr
#include <wrl.h>
#include "DirectXCommon.h"

class SpriteCommon;

/// <summary>
/// スプライト
/// </summary>
class Sprite{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="textureFilePath">テクスチャ名</param>
	void Initialize(const std::string& textureFilePath);
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
	const Vector2& GetPosition()const { return position_; }
	/// <summary>
	/// setter_座標
	/// </summary>
	/// <param name="position">指定する座標</param>
	void SetPosition(const Vector2& position) { this->position_ = position; }

	/// <summary>
	/// getter_回転角度
	/// </summary>
	/// <returns>現在の回転角度</returns>
	float GetRotate() const { return rotation_; }
	/// <summary>
	/// setter_回転角度
	/// </summary>
	/// <param name="rotation">指定した回転角度</param>
	void SetRotate(float rotation) { this->rotation_ = rotation; }

	/// <summary>
	/// getter_色
	/// </summary>
	/// <returns>現在の色</returns>
	const Vector4& GetColor()const { return materialData_->color; }
	/// <summary>
	/// setter_色
	/// </summary>
	/// <param name="color">指定する色(Vector4)</param>
	void SetColor(const Vector4& color) { materialData_->color = color; }
	/// <summary>
	/// getter_サイズ
	/// </summary>
	/// <returns>現在のサイズ</returns>
	const Vector2& GetSize()const { return size_; }
	/// <summary>
	/// setter_サイズ
	/// </summary>
	/// <param name="size">指定する大きさ</param>
	void SetSize(const Vector2& size) { this->size_ = size; }

	/// <summary>
	/// getter_アンカーポイント
	/// </summary>
	/// <returns>現在のアンカーポイント</returns>
	const Vector2& GetAnchorPoint()const { return anchorPoint_; }
	
	/// <summary>
	/// setter_アンカーポイント
	/// </summary>
	/// <param name="anchorPoint">指定するアンカーポイント座標</param>
	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint_ = anchorPoint; }

	/// <summary>
	/// getter_テクスチャ反転横
	/// </summary>
	/// <returns>現在のテクスチャ反転横</returns>
	const bool& GetFlipX() const { return isFlipX_; }

	/// <summary>
	/// setter_テクスチャ反転横
	/// </summary>
	/// <returns>指定するテクスチャ反転横</returns>
	void SetFlipX(bool isFlipX) { this->isFlipX_ = isFlipX; }

	/// <summary>
	/// getter_テクスチャ反転縦
	/// </summary>
	/// <returns>現在のテクスチャ反転縦</returns>
	const bool& GetFlipY() const { return isFlipY_; }

	/// <summary>
	/// setter_テクスチャ反転縦
	/// </summary>
	/// <returns>指定するテクスチャ反転縦</returns>
	void SetFlipY(bool isFlipY) { this->isFlipY_ = isFlipY; }

	/// <summary>
	/// getter_左上の位置
	/// </summary>
	/// <returns>現在の左上の位置</returns>
	const Vector2& GetTextureLT() const { return textureLeftTop_; }

	/// <summary>
	/// setter_左上の位置
	/// </summary>
	/// <param name="textureLeftTop">指定する左上の位置</param>
	void SetTextureLT(const Vector2& textureLeftTop) { this->textureLeftTop_ = textureLeftTop; }
	
	/// <summary>
	/// getter_テクスチャサイズ
	/// </summary>
	/// <returns>現在のテクスチャサイズ</returns>
	const Vector2& GetTextureSize() const { return textureSize_; }

	/// <summary>
	/// setter_テクスチャサイズ
	/// </summary>
	/// <param name="textureSize">テクスチャ自体のサイズ</param>
	void SetTextureSize(const Vector2& textureSize) { this->textureSize_ = textureSize; }

	/// <summary>
	/// getter_テクスチャファイル
	/// </summary>
	/// <returns>現在使用しているテクスチャファイル名</returns>
	std::string GetTextureFile() const;

	/// <summary>
	/// setter_テクスチャファイル
	/// </summary>
	/// <param name="textureFile">変更するテクスチャファイルパス</param>
	void SetTextureFile(const std::string& textureFile);


	/// <summary>
	/// getter_SpriteのResource
	/// </summary>
	/// <returns>使用しているハンドルGPU</returns>
	const D3D12_GPU_DESCRIPTOR_HANDLE GetResource() const ;

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;

	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	//頂点バッファービュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;


	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//マテリアルにデータを書き込む
	Material* materialData_ = nullptr;


	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;

	TransformationMatrix* transformationMatrixData_ = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_;

	Transform transform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,0.0f} };
	
	Transform uvTransform_{
	{ 1.0f,1.0f,1.0f },
	{ 0.0f,0.0f,0.0f },
	{ 0.0f,0.0f,0.0f }
	};


	//座標
	Vector2 position_ = { 0.0f,0.0f };
	
	float rotation_ = 0.0f;

	Vector2 size_ = { 100.0f,100.0f };


	uint32_t textureIndex_ = 0;

	//アンカーポイント ココが中心点になる
	Vector2 anchorPoint_ = { 0.0f,0.0f };
	
	//フリップ テクスチャ反転
	bool isFlipX_ = false;
	bool isFlipY_ = false;

	//左上から情報を得る
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	//切り取りサイズ
	Vector2 textureSize_ = { 64.0f,64.0f };

	std::string filePath_;

	/// <summary>
	/// 本来のテクスチャの大きさにする
	/// </summary>
	void AdjustTextureSize();

	/// <summary>
	/// テクスチャサイズ更新処理
	/// </summary>
	void TextureSizeUpdate();

	/// <summary>
	/// vertexResourceの初期化
	/// </summary>
	void InitVertexResource();

	/// <summary>
	/// vertexDataの初期化
	/// </summary>
	void InitVertexData();

	/// <summary>
	/// indexDataの初期化
	/// </summary>
	void InitIndexData();

	/// <summary>
	/// マテリアルの初期化
	/// </summary>
	void InitMaterial();

	/// <summary>
	/// 座標変換行列の初期化
	/// </summary>
	void InitTransformationMatrix();

};
