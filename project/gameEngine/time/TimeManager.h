#pragma once

#include <chrono>
#include <functional>
#include <vector>
#include <cstdint>

using TimerId = uint64_t;

class TimeManager
{
public:

	// シングルトン
	static TimeManager& Instance();

	/// <summary>
	/// タイムマネージャーの更新
	/// 呼び出しごとにデルタタイムを計算し、タイマーやタイムスケールの更新を行う
	/// </summary>
	void Tick();

	/// <summary>
	/// スムーズなタイムスケール変更をキャンセルする
	/// </summary>
	void CancelSmoothTimeScale();

	/// <summary>
	/// 時間の一時停止
	/// </summary>
	void Pause();
	/// <summary>
	/// 時間の再開
	/// </summary>
	void Resume();
	/// <summary>
	/// 一時停止中でも時間を進めるステップ実行
	/// </summary>
	/// <param name="seconds">進める秒数</param>
	void Step(float seconds);

	/// <summary>
	/// タイマーのキャンセル
	/// </summary>
	/// <param name="id">キャンセルするタイマーのID</param>
	void CancelTimer(TimerId id);

	/// <summary>
	/// スムーズなタイムスケール変更の設定
	/// </summary>
	/// <param name="toScale">目標のタイムスケール</param>
	/// <param name="durationSeconds">変更にかける時間(秒)</param>
	void SmoothTimeScale(float toScale, float durationSeconds);

	/// <summary>
	/// タイムスケール変更コールバックの追加
	/// </summary>
	using TimeScaleCallback = std::function<void(float newScale)>;
	/// <summary>
	/// タイムスケール変更コールバックの追加
	/// </summary>
	/// <param name="cb">コールバック関数</param>
	void AddTimeScaleCallback(TimeScaleCallback cb);

public: // セッター

	/// <summary>
	/// 固定デルタタイムの設定
	/// </summary>
	/// <param name="dt">固定デルタタイム</param>
	void SetFixedDeltaTime(float dt)
	{
		fixedDeltaTime_ = dt;
	}
	/// <summary>
	/// 固定デルタタイムがタイムスケールの影響を受けるかどうかの設定
	/// </summary>
	/// <param name="useScaled">影響を受けるならtrue</param>
	void SetFixedUsesScaled(bool useScaled)
	{
		fixedUsesScaled_ = useScaled;
	}

	/// <summary>
	/// タイマーの設定
	/// </summary>
	/// <param name="cb">コールバック関数</param>
	/// <param name="seconds">遅延時間(秒)</param>
	/// <param name="useUnscaled">タイムスケールの影響を受けないならtrue</param>
	/// <returns>タイマーID</returns>
	TimerId SetTimeout(std::function<void()> cb, float seconds, bool useUnscaled = false)
	{
		Timer t;
		t.id = nextTimerId_++;
		t.cb = std::move(cb);
		t.remaining = seconds;
		t.interval = 0.0f;
		t.useUnscaled = useUnscaled;
		timers_.push_back(std::move(t));
		return t.id;
	}
	/// <summary>
	/// インターバルタイマーの設定
	/// </summary>
	/// <param name="cb">コールバック関数</param>
	/// <param name="seconds">遅延時間(秒)</param>
	/// <param name="useUnscaled">タイムスケールの影響を受けないならtrue</param>
	/// <returns>タイマーID</returns>
	TimerId SetInterval(std::function<void()> cb, float seconds, bool useUnscaled = false)
	{
		Timer t;
		t.id = nextTimerId_++;
		t.cb = std::move(cb);
		t.remaining = seconds;
		t.interval = seconds;
		t.useUnscaled = useUnscaled;
		timers_.push_back(std::move(t));
		return t.id;
	}

	/// <summary>
	/// タイムスケールの設定
	/// </summary>
	/// <param name="timeScale">新しいタイムスケール</param>
	void SetTimeScale(float scale)
	{
		timeScale_ = std::max(0.0f, scale);
		smoothingEnabled_ = false;
		for (auto& cb : timeScaleCallbacks_) if (cb) cb(timeScale_);
	}
	

public: // ゲッター

	/// <summary>
	/// 時間が一時停止中かどうか
	/// </summary>
	/// <returns>停止中ならtrue</returns>
	bool IsPaused() const
	{
		return paused_;
	}

	// デルタタイム取得
	float GetDeltaTime() const { return deltaTime_; }
	// スケールされていないデルタタイム取得
	float GetUnscaledDeltaTime() const { return unscaledDeltaTime_; }

	// タイムスケール操作
	float GetTimeScale() const { return timeScale_; }

	// 固定デルタタイム操作
	float GetFixedDeltaTime() const { return fixedUsesScaled_ ? fixedDeltaTime_ * timeScale_ : fixedDeltaTime_; }
	// 固定デルタタイムがスケールの影響を受けるかどうか
	bool GetFixedUsesScaled() const { return fixedUsesScaled_; }

private:

	// コンストラクタ(シングルトン)
	TimeManager();
	// タイムスケールの更新
	void UpdateTimers(float scaledDT, float unscaledDT);
	
	// デルタタイムの更新
	struct Timer
	{
		TimerId id = 0;
		std::function<void()> cb;
		float remaining = 0.0f;
		float interval = 0.0f;
		bool useUnscaled = false;
		bool cancelled = false;
	};

	// スムーズなタイムスケール変更の状態
	bool smoothingEnabled_ = false;
	// スムーズ変更の開始時のタイムスケール
	float smoothStart_ = 1.0f;
	// スムーズ変更の目標タイムスケール
	float smoothTarget_ = 1.0f;
	// スムーズ変更にかける時間(秒)
	float smoothDuration_ = 0.0f;
	// スムーズ変更の経過時間(秒)
	float smoothElapsed_ = 0.0f;

	// 現在のタイムスケール
	float timeScale_ = 1.0f;
	// デルタタイム
	float deltaTime_ = 1.0f / 60.0f;
	// スケールされていないデルタタイム
	float unscaledDeltaTime_ = 1.0f / 60.0f;

	// 固定デルタタイム
	float fixedDeltaTime_ = 1.0f / 60.0f;
	// 固定デルタタイムがタイムスケールの影響を受けるかどうか
	bool fixedUsesScaled_ = true;

	// 一時停止中かどうか
	bool paused_ = false;
	// ステップ実行用の時間蓄積
	float stepAccumulator_ = 0.0f;

	// 前回の更新時間
	std::chrono::steady_clock::time_point lastTime_;
	// 初期化済みかどうか
	bool initialized_ = false;

	// 登録されているタイマー
	std::vector<Timer> timers_;
	// 次に割り当てるタイマーID
	TimerId nextTimerId_ = 1;

	// タイムスケール変更コールバック群
	std::vector<TimeScaleCallback> timeScaleCallbacks_;

};

