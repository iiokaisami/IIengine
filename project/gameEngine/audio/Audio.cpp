#include "Audio.h"
#include <cassert>
#include <cstring>

Audio* Audio::GetInstance()
{
	static Audio instance;
	return &instance;
}

void Audio::Finalize()
{
	if (masterVoice_) 
	{
		masterVoice_->DestroyVoice();
		masterVoice_ = nullptr;
	}
}

void Audio::Initialize()
{
	// xAudio生成
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	// マスターボイス生成
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
}

SoundData Audio::LoadWav(const char* filename)
{
	std::ifstream file;
	// 基本パスを指定（"resources/audio/"）
	std::string basePath = "resources/audio/";
	std::string fullPath = basePath + filename;

	// .wavファイルをバイナリモードで展開
	file.open(fullPath.c_str(), std::ios_base::binary);
	// ファイル展開失敗時
	assert(file.is_open());

	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) 
	{
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) 
	{
		assert(0);
	}

	// チャンクのループを開始
	ChunkHeader chunkHeader;
	FormatChunk format = {};
	while (file.read((char*)&chunkHeader, sizeof(chunkHeader))) {
		// チャンクIDが "fmt" か確認
		if (strncmp(chunkHeader.id, "fmt ", 4) == 0) {
			// Formatチャンクのサイズを確認、データを読み込む
			assert(chunkHeader.size <= sizeof(format.fmt));

			format.chunk = chunkHeader; // チャンクヘッダーをコピー
			file.read((char*)&format.fmt, chunkHeader.size); // fmtのデータを読み込み

			break;
		}
		else {
			// 次のチャンクに移動
			file.seekg(chunkHeader.size, std::ios_base::cur);
		}
	}

	// "fmt"チャンクが見つからなかった場合のエラーとしてだす
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}


	// Dataチャンクの読み込みとスキップ処理
	ChunkHeader data;
	while (file.read((char*)&data, sizeof(data))) {
		if (strncmp(data.id, "data", 4) == 0) {
			break; // "data" チャンクを見つけたらループを抜ける
		}
		else 
		{
			// 現在のチャンクが "data" でない場合、そのサイズ分シーク
			file.seekg(data.size, std::ios_base::cur);
		}
	}


	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部（波形データの読み込み）
	std::vector<char> tmpBuffer(data.size);
	file.read(tmpBuffer.data(), data.size);

	// Wavファイルを閉じる
	file.close();


	// returnする為のデータ
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer.resize(data.size);
	std::memcpy(soundData.pBuffer.data(), tmpBuffer.data(), data.size);
	soundData.bufferSize = data.size;

	soundData.sourceVoice = nullptr;

	return soundData;
}

void Audio::SoundUnload(Microsoft::WRL::ComPtr<IXAudio2> xAudio2, SoundData* soundData)
{
	if (soundData->sourceVoice) {
		soundData->sourceVoice->Stop();
		soundData->sourceVoice->FlushSourceBuffers();
		soundData->sourceVoice->DestroyVoice();
		soundData->sourceVoice = nullptr;
	}

	// 音声データ解放
	xAudio2.Reset();

	soundData->pBuffer.clear();
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Audio::PlayWave(SoundData& soundData, bool loop, float volume)
{
	// 波形フォーマットをもとにSourceVoiceの生成
	hr = xAudio2_->CreateSourceVoice(&soundData.sourceVoice, &soundData.wfex);
	assert(SUCCEEDED(hr));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer.data();
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// ボリュームを設定
	soundData.sourceVoice->SetVolume(volume);

	// "loop" がtrueの場合ループさせる
	if (loop) {
		buf.LoopCount = XAUDIO2_LOOP_INFINITE; // 無限ループ
	}

	// 波形データの再生
	hr = soundData.sourceVoice->SubmitSourceBuffer(&buf);
	hr = soundData.sourceVoice->Start();
}

void Audio::SoundStop(SoundData& soundData)
{
	if (soundData.sourceVoice) {
		soundData.sourceVoice->Stop();
		soundData.sourceVoice->FlushSourceBuffers();
		soundData.sourceVoice->DestroyVoice();
		soundData.sourceVoice = nullptr;
	}
}

void Audio::SetVolume(SoundData& soundData, float volume)
{
	// 0.0f ~ 1.0f の範囲で音量を設定
	hr = soundData.sourceVoice->SetVolume(volume);

	// 失敗時にエラーを出力
	assert(SUCCEEDED(hr) && "Failed to set volume");
}
