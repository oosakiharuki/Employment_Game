#pragma once
#include "Input.h"
#include "DirectXCommon.h"
#include "SrvManager.h"

#include "SpriteCommon.h"
#include "Object3dCommon.h"
#include "GLTFCommon.h"
#include "SkinningCommon.h"
#include "ParticleCommon.h"

#include "ModelManager.h"
#include "ParticleManager.h"
#include "TextureManager.h"

#include "ImGuiManager.h"
#include "D3DResourceLeakChecker.h"

#include "PostEffectManager.h"

#include "Audio.h"

#include"DebugWireframes.h"
#include "CubeMap.h"
/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	/// <summary>
	/// エンジンをまとめたもの
	/// </summary>
	class Framework {
	public:
		/// <summary>
		/// 初期化処理
		/// </summary>
		virtual void Initialize();
		/// <summary>
		/// 更新処理
		/// </summary>
		virtual void Update();
		/// <summary>
		/// 描画処理
		/// </summary>
		virtual void Draw() = 0;
		/// <summary>
		/// 解放処理
		/// </summary>
		virtual void Finalize();
		/// <summary>
		/// ゲームを終了する
		/// </summary>
		/// <returns>trueで終了</returns>
		virtual bool IsEndRequest() { return isRequest_; }
		/// <summary>
		/// ゲームを終了するか
		/// </summary>
		/// <param name="result">終了フラグ</param>
		virtual void SetIsEndRequest(bool result) { isRequest_ = result; }

		virtual ~Framework() = default;
		/// <summary>
		/// 繰り返し処理
		/// </summary>
		void Run();
	private:
		std::unique_ptr<WinApp> winApp_ = nullptr;

		bool isRequest_ = false;

		/// <summary>
		/// オブジェクト(2d)の共通処理のまとめ
		/// </summary>
		void SpriteCommons();
		/// <summary>
		/// スプライト(3d)の共通処理のまとめ
		/// </summary>
		void ObjectCommons();
	};
}