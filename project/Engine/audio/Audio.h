#pragma once

#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include <fstream>

//ComPtr
#include <wrl.h>
#include <cassert>

#include <vector>
#include <unordered_map>

/// <summary>
/// エンジン層
/// </summary>
namespace EngineLayer {
	/// <summary>
	/// チャンク
	/// </summary>
	struct ChunkHeader {
		char id[4];//チャンク毎ID
		int32_t size;//サイズ
	};
	/// <summary>
	/// Wave読み取りパラメータ
	/// </summary>
	struct RiffHeader {
		ChunkHeader chunk;//RIFF
		char type[4];//WAVE
	};

	struct FormatChunk {
		ChunkHeader chunk;
		WAVEFORMATEX fmt;
	};
	/// <summary>
	/// サウンドデータ
	/// </summary>
	struct SoundData {
		//波形フォーマット
		WAVEFORMATEX wfex;
		//バッファ
		std::vector<BYTE> buffer;
		//ソースボイス
		IXAudio2SourceVoice* pSourceVoice = nullptr;
	};
	/// <summary>
	/// サウンド
	/// </summary>
	class Audio {
	public:
		/// <summary>
		/// インスタンス生成
		/// </summary>
		/// <returns>インスタンス</returns>
		static Audio& GetInstance();
		/// <summary>
		/// 初期化処理
		/// </summary>
		void Initialize();
		/// <summary>
		/// 解放処理
		/// </summary>
		void Finalize();
		/// <summary>
		/// 音声をロードする
		/// </summary>
		/// <param name="filename">waveファイル名</param>
		/// <returns>ロードされたサウンドデータ</returns>
		void LoadWave(const std::string& filename);

		/// <summary>
		/// 音声を再生
		/// </summary>
		/// <param name="soundData">流したい音声データの名前</param>
		/// <param name="volume">音量</param>
		/// <param name="isLoop">ループするか</param>
		void SoundPlayWave(const std::string& soundDataName, float volume, bool isLoop = false);

		/// <summary>
		/// 音声はすでに鳴っているか
		/// </summary>
		/// <param name="soundDataName"></param>
		/// <returns></returns>
		bool IsPlayingSound(const std::string& soundDataName);

		/// <summary>
		/// 音声を停止
		/// </summary>
		/// <param name="soundDataName">止めたい音声データ</param>
		void StopWave(const std::string& soundDataName);
		/// <summary>
		/// 音声のボリューム
		/// </summary>
		/// <param name="soundDataName">変えたい音声データ</param>
		/// <param name="volume">音量</param>
		void ControlVolume(const std::string& soundDataName, float volume);
	private:

		/// <summary>
		/// 音声ファイルを読み取る
		/// </summary>
		/// <param name="fileName">音声ファイル名</param>
		SoundData SoundLoadFile(const std::string& fileName);

		/// <summary>
		/// 音声データの解放 delete
		/// </summary>
		/// <param name="soundData">サウンドデータ</param>
		void SoundUnload();

		//インスタンス
		static std::unique_ptr<Audio> sInstance_;
		//default_deleteを設定(解放処理を行える)
		friend struct std::default_delete<Audio>;

		std::unordered_map<std::string, SoundData> soundDates_;

		//audio
		Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;

		IXAudio2MasteringVoice* masterVoice_;

		HRESULT result_;
	};
}