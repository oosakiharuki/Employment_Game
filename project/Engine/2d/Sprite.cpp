#include "Sprite.h"
#include "SpriteCommon.h"
#include "externals/imgui/imgui.h"
#include "TextureManager.h"

using namespace MyMath;

void Sprite::Initialize(const std::string& textureFilePath) {
	this->spriteCommon_ = SpriteCommon::GetInstance().get();
	//テクスチャ名
	filePath_ = "resource/Sprite/" + textureFilePath;

	TextureManager::GetInstance()->LoadTexture(filePath_);
	
	//vertexResourceの初期化
	InitVertexResource();

	//vertexDataの初期化
	InitVertexData();

	//indexDataの初期化
	InitIndexData();

	//マテリアルの初期化
	InitMaterial();

	//座標変換行列の初期化
	InitTransformationMatrix();
}

void Sprite::InitVertexResource() {
	//Sprite
	vertexResource_ = spriteCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(VertexData) * 4);

	//リソースの先頭アドレス
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	//頂点サイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
}

void Sprite::InitVertexData() {
	float left = 0.0f - anchorPoint_.x;
	float right = 1.0f - anchorPoint_.x;
	float top = 0.0f - anchorPoint_.y;
	float bottom = 1.0f - anchorPoint_.y;

	//左右反転
	if (isFlipX_) {
		left = -left;
		right = -right;
	}

	//上下反転
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	AdjustTextureSize();
	const DirectX::TexMetadata& kMetadata = TextureManager::GetInstance()->GetMetaData(filePath_);
	float tex_left = textureLeftTop_.x / kMetadata.width;
	float tex_right = textureLeftTop_.x + textureSize_.x / kMetadata.width;
	float tex_top = textureLeftTop_.y / kMetadata.height;
	float tex_bottom = textureLeftTop_.y + textureSize_.y / kMetadata.height;

	vertexData_[0].position = { left,bottom,0.0f,1.0f };//0
	vertexData_[0].texcoord = { tex_left,tex_bottom };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };

	vertexData_[1].position = { left,top,0.0f,1.0f };//1,3
	vertexData_[1].texcoord = { tex_left,tex_top };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };

	vertexData_[2].position = { right,bottom,0.0f,1.0f };//2,5
	vertexData_[2].texcoord = { tex_right,tex_bottom };
	vertexData_[2].normal = { 0.0f,0.0f,-1.0f };

	vertexData_[3].position = { right,top,0.0f,1.0f };//4
	vertexData_[3].texcoord = { tex_right,tex_top };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };
}

void Sprite::InitIndexData() {

	//Index
	indexResource_ = spriteCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(uint32_t) * 6);

	//リソースの先頭アドレス
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	//使用するリソースサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	//頂点サイズ
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;
}

void Sprite::InitMaterial() {
	//spriteのリソース
	materialResource_ = spriteCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(Material));

	//書き込むためのアドレス
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//色の設定
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentity4x4();
}

void Sprite::InitTransformationMatrix() {
	//座標変換行列	
	transformationMatrixResource_ = spriteCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(TransformationMatrix));

	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));

	transformationMatrixData_->WVP = MakeIdentity4x4();
	transformationMatrixData_->World = MakeIdentity4x4();
}

void Sprite::AdjustTextureSize() {
	const DirectX::TexMetadata& kMetadata = TextureManager::GetInstance()->GetMetaData(filePath_);

	textureSize_.x = static_cast<float>(kMetadata.width);
	textureSize_.y = static_cast<float>(kMetadata.height);

	size_ = textureSize_;
}


void Sprite::Update() {

	transform_.translate = { position_.x,position_.y,0.0f };
	transform_.rotate = { 0.0f,0.0f,rotation_ };
	transform_.scale = { size_.x,size_.y,1.0f };

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, (float)WinApp::kClientWidth_, (float)WinApp::kClientHeight_, 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

	transformationMatrixData_->WVP = worldViewProjectionMatrix;
	transformationMatrixData_->World = worldMatrix;
}

void Sprite::Draw() {
	spriteCommon_->GetDirectXCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	spriteCommon_->GetDirectXCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
	spriteCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress()); //rootParameterの配列の0番目 [0]
	spriteCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	spriteCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(filePath_));
	spriteCommon_->GetDirectXCommon()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::SetTextureFile(const std::string& textureFile) {
	filePath_ = "resource/Sprite/" + textureFile;
	TextureManager::GetInstance()->LoadTexture(filePath_);
}

std::string Sprite::GetTextureFile() const {
	std::string result = filePath_;
	size_t fileName = result.rfind("/");// Sprite "/" ○○.png
	//ファイル名を入れる
	result = result.substr(fileName + 1,result.size() - fileName - 1);
	return result;
}


const D3D12_GPU_DESCRIPTOR_HANDLE Sprite::GetResource() const {
	return TextureManager::GetInstance()->GetSrvHandleGPU(filePath_);
}
