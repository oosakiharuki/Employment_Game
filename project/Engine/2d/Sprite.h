#pragma once
//#include "Vector2.h"
//#include "Vector3.h"
//#include "Vector4.h"
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
	void Initialize(std::string textureFilePath);
	void Update();
	void Draw();


	const Vector2& GetPosition()const { return position_; }
	void SetPosition(const Vector2& position) { this->position_ = position; }

	float GetRotate() { return rotation_; }
	void SetRotate(float rotation) { this->rotation_ = rotation; }

	const Vector4& GetColor()const { return materialData_->color; }
	void SetColor(const Vector4& color) { materialData_->color = color; }

	const Vector2& GetSize()const { return size_; }
	void SetSize(const Vector2& size) { this->size_ = size; }


	const Vector2& GetAnchorPoint()const { return anchorPoint_; }
	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint_ = anchorPoint; }

	const bool& GetFlipX() const { return isFlipX_; }
	const bool& GetFlipY() const { return isFlipY_; }
	
	void SetFlipX(const bool& isFlipX) { this->isFlipX_ = isFlipX; }
	void SetFlipY(const bool& isFlipY) { this->isFlipY_ = isFlipY; }


	const Vector2& GetTextureLT() const { return textureLeftTop_; }
	const Vector2& GetTextureSize() const { return textureSize_; }

	void SetTextureLT(const Vector2& textureLeftTop) { this->textureLeftTop_ = textureLeftTop; }
	void SetTextureSize(const Vector2& textureSize) { this->textureSize_ = textureSize; }

	void SetTextureFile(const std::string textureFile);
	std::string GetTextureFile();

	D3D12_GPU_DESCRIPTOR_HANDLE GetResource();

private:
	SpriteCommon* spriteCommon_ = nullptr;


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

	//本来のテクスチャの大きさにする
	void AdjustTextureSize();

	std::string filePath_;
};
