#pragma once

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	/// <summary>
	/// DirectXで解放
	/// </summary>
	class D3DResourceLeakChecker {
	public:
		///<summary>
		///デストラクタ
		///</summary>
		~D3DResourceLeakChecker();
	};
}