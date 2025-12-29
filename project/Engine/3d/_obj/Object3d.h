#pragma once
#include "Model_obj.h"
#include "BaseObject.h"

class Object3dCommon;
/// <summary>
/// .obj版のオブジェクト
/// </summary>
class Object3d : public BaseObject
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const WorldTransform& worldTransform) override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(const std::string& textureData) override;

	void SetModel(Model_obj* model) { this->model_ = model; }

	/// <summary>
	/// setter_modelの選択
	/// </summary>
	/// <param name="filePath">ファイル名</param>
	void SetModelFile(const std::string& filePath) override;
	/// <summary>
	/// ライトの設定
	/// </summary>
	/// <param name="isLight">on/off</param>
	void LightSwitch(bool isLight) override;
	/// <summary>
	/// 色の変更
	/// </summary>
	/// <param name="color">変更カラー</param>
	void SetColor(const Vector4& color) override;

private:
	Object3dCommon* object3dCommon_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_;
	TransformationMatrix* wvpData_ = nullptr;

	Model_obj* model_ = nullptr;
};