#pragma once

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <fstream>
#include <vector>
#include <wrl/client.h>

// チャンクヘッダ
struct ChunkHeader {
	char id[4];		 // チャンク毎のID
	int32_t size;	 // チャンクサイズ
};
// RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk;	// "RIFF"
	char type[4];		// "WAVE"
};
// FMTチャンク
struct FormatChunk {
	ChunkHeader chunk;	// "fmt"
	WAVEFORMATEX fmt;	// 波形フォーマット
};
// 音声データ
struct SoundData {
	WAVEFORMATEX wfex;		// 波形フォーマット
	std::vector<BYTE> pBuffer;			// バッファの先頭アドレス
	IXAudio2SourceVoice* sourceVoice = nullptr;  // ソースボイス
	unsigned int bufferSize;// バッファサイズ
};

/// <summary>
/// オーディオ
/// 音声再生、管理を行う
/// </summary>
class Audio
{
#pragma region シングルトンインスタンス
private:

	Audio() = default;
	~Audio() = default;
	Audio(Audio&) = delete;
	Audio& operator = (Audio&) = delete;

public:
	// シングルトンインスタンスの取得
	static Audio* GetInstance();
	// 終了
	void Finalize();
#pragma endregion シングルトンインスタンス

public:
	
	// 初期化
	void Initialize();
	
	/// <summary>
	/// 音声データ読み込み
	/// </summary>
	/// <param name="filename">読み込むWAVファイルのパス</param>
	/// <returns>読み込んだ音声データ</returns>
	SoundData LoadWav(const char* filename);

	/// <summary>
	/// 音声データの解放
	/// </summary>
	/// <param name="xAudio2"> XAudio2インスタンス</param>
	/// <param name="soundData">解放したい音声データ</param>
	void SoundUnload(Microsoft::WRL::ComPtr<IXAudio2> xAudio2, SoundData* soundData);

	/// <summary>
	/// サウンド再生
	/// </summary>
	/// <param name="soundData"> 再生したいサウンドデータ</param>
	/// <param name="loop">ループするか</param>
	/// <param name="volume">ボリューム</param>
	void PlayWave(SoundData& soundData, bool loop, float volume);

	/// <summary>
	/// サウンド停止 
	/// </summary>
	/// <param name="soundData">停止したいサウンドデータ</param>
	void SoundStop(SoundData& soundData);

	/// <summary>
	/// ボリュームを設定 
	/// </summary>
	/// <param name="soundData">設定したいサウンドデータ</param>
	/// <param name="volume">ボリューム</param>
	void SetVolume(SoundData& soundData, float volume);

public: // ゲッター

	// xAudioの取得
	Microsoft::WRL::ComPtr<IXAudio2> GetXAudio2() const { return xAudio2_; }

private:

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = nullptr;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;

	HRESULT hr;
};

