#pragma once
#include "wrl.h"
#include "d3d12.h"
#include <dxcapi.h>
#include <string>
#include <memory>

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {

	/// <summary>
	/// シェーダーマネージャ
	/// </summary>
	class ShaderManager {
	public:

		/// <summary>
		/// インスタンス生成
		/// </summary>
		/// <returns></returns>
		static ShaderManager& GetInstance();

		/// <summary>
		/// 初期化処理 (DXCの初期化)
		/// </summary>
		void Initialize();

		/// <summary>
		/// コンパイルシェーダ作成
		/// </summary>
		/// <param name="filePath">HLSLの名前</param> 
		/// <param name="profile">vs_6_0 or ps_6_0</param> 
		/// <returns>コンパイルシェーダ</returns>
		Microsoft::WRL::ComPtr <IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);

		/// <summary>
		/// インスタンス解放キー
		/// </summary>
		class InstanceKey {
		private:
			InstanceKey() = default;
			friend class ShaderManager;
		};
		explicit ShaderManager(InstanceKey) {}

	private:

		/// <summary>
		/// HLSLを読み取る
		/// </summary>
		/// <param name="filePath">HLSLパス</param>
		/// <param name="profile">プロファイル</param>
		void LoadHLSL(const std::wstring& filePath, const wchar_t* profile);
		/// <summary>
		/// 警告エラー
		/// </summary>
		void CompileError();
		/// <summary>
		/// コンパイルが完了した
		/// </summary>
		/// <param name="filePath">HSLSパス</param>
		/// <param name="profile">プロファイル</param>
		void CompileSuccess(const std::wstring& filePath, const wchar_t* profile);

		Microsoft::WRL::ComPtr<IDxcResult> shaderResult = nullptr;
		Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = nullptr;

		//DXC
		Microsoft::WRL::ComPtr <IDxcUtils> dxcUtils_ = nullptr;
		Microsoft::WRL::ComPtr <IDxcCompiler3> dxcCompiler_ = nullptr;
		Microsoft::WRL::ComPtr <IDxcIncludeHandler> includeHandler_ = nullptr;




		static std::unique_ptr<ShaderManager> sInstance_;
	};
}