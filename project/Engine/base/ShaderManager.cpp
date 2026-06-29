#include "ShaderManager.h"
#include "Logger.h"
#include "StringUtility.h"
#include <format>
#include <cassert>

using namespace Logger;
using namespace StringUtility;

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {

	std::unique_ptr<ShaderManager> ShaderManager::sInstance_ = nullptr;

	ShaderManager& ShaderManager::GetInstance() {
		if (sInstance_ == nullptr) {
			sInstance_ = std::make_unique<ShaderManager>(InstanceKey());
		}
		return *sInstance_;
	}

	void ShaderManager::Initialize() {
		//DXC初期化
		HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
		assert(SUCCEEDED(hr));

		hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
		assert(SUCCEEDED(hr));

		hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
		assert(SUCCEEDED(hr));
	}

	
	//ComplierShader関数
	Microsoft::WRL::ComPtr<IDxcBlob> ShaderManager::CompileShader(const std::wstring& filePath, const wchar_t* profile)
	{
		//1.HLSLファイル
		LoadHLSL(filePath, profile);

		//3.警告エラー
		CompileError();

		//4.Compile結果
		CompileSuccess(filePath, profile);

		return shaderBlob;
	}

	void ShaderManager::LoadHLSL(const std::wstring& filePath, const wchar_t* profile) {
		//ログ
		log(ConvertString(std::format(L"Begin CompileShader path:{},profile{}\n", filePath, profile)));

		Microsoft::WRL::ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
		HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(),nullptr,&shaderSource);//hlslを読み取る

		//ない場合エラー
		assert(SUCCEEDED(hr));

		//内容の設定
		DxcBuffer shaderSourceBuffer;
		shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
		shaderSourceBuffer.Size = shaderSource->GetBufferSize();
		shaderSourceBuffer.Encoding = DXC_CP_UTF8;

		LPCWSTR arguments[] = {
			filePath.c_str(),        //コンパイル対象
			L"-E",L"main",           //エントリーポイント
			L"-T",profile,           //shaderProfile
			L"-Zi",L"-Qembed_debug", //デバッグ用の情報を読み取る
			L"-Od",                  //最適化なし
			L"-Zpr",                 //メモリレイアウト
		};

		//コンパイルする
		hr = dxcCompiler_->Compile(
			&shaderSourceBuffer,
			arguments,
			_countof(arguments),
			*&includeHandler_,
			IID_PPV_ARGS(&shaderResult)
		);

		//dxcが起動できるか
		assert(SUCCEEDED(hr));
	}


	void ShaderManager::CompileError() {
		Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError = nullptr;
		shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
		if (shaderError != nullptr && shaderError->GetStringLength() != 0)
		{
			log(shaderError->GetStringPointer());
			//警告エラーダメ絶対
			assert(false);
		}
	}

	void ShaderManager::CompileSuccess(const std::wstring& filePath, const wchar_t* profile) {
		HRESULT hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
		assert(SUCCEEDED(hr));
		//成功ログ
		log(ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	}

}