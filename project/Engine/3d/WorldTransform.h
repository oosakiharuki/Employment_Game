#pragma once
#include"Vector3.h"
#include "Matrix4x4.h"
#include "Transform.h"

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	/// <summary>
	/// ワールド座標
	/// </summary>
	class WorldTransform {
	public:

		WorldTransform();
		~WorldTransform();

		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize();
		/// <summary>
		/// Transform更新処理
		/// </summary>
		const Transform& UpdateTransform() { return transform_; }
		/// <summary>
		/// 更新処理
		/// </summary>
		/// <param name="transform">トランスフォーム</param>
		void UpdateMatrix(const Transform& transform);
		/// <summary>
		/// getter_ワールド行列(座標変換行列)
		/// </summary>
		/// <returns>現在のワールド座標</returns>
		const Matrix4x4& GetMatWorld() { return matWorld_; }
		/// <summary>
		/// setter_ワールド行列(座標変換行列)
		/// </summary>
		/// <param name="matWorld">ワールド座標</param>
		void SetMatWorld(const Matrix4x4& matWorld) { matWorld_ = matWorld; }
		/// <summary>
		/// getter_親子関係
		/// </summary>
		/// <returns>親</returns>
		const WorldTransform* GetParent() { return parent_; }
		/// <summary>
		/// setter_親子関係
		/// </summary>
		/// <param name="wt">親子関係になりたいWorldTransform(自分は子)</param>
		void SetParent(const WorldTransform& wt) { parent_ = &wt; }
		/// <summary>
		/// 親子関係の解除
		/// </summary>
		void ResetParent() { parent_ = nullptr; }
	private:

		Transform transform_;

		Matrix4x4 matWorld_;
		const WorldTransform* parent_ = nullptr;
	};
}