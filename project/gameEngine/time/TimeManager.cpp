#include "TimeManager.h"
#include <algorithm>

TimeManager& TimeManager::Instance()
{
	// シングルトンインスタンスの取得
	static TimeManager instance;
	return instance;
}

TimeManager::TimeManager()
{
	// 初期化
    lastTime_ = std::chrono::steady_clock::now();
    initialized_ = true;
}

void TimeManager::UpdateTimers(float scaledDT, float unscaledDT)
{
	// 早期リターン
    if (timers_.empty())
    {
        return;
    }

	// コールバック呼び出し用リスト
    std::vector<std::function<void()>> calls;
	// タイマー更新ループ
    for (auto it = timers_.begin(); it != timers_.end();)
    {
		// タイマー参照
        Timer& t = *it;
        if (t.cancelled) 
        {
            it = timers_.erase(it); continue; 
        }

		// デルタタイム適用
        float dt = t.useUnscaled ? unscaledDT : scaledDT;
        t.remaining -= dt;
		// タイマー完了判定
        if (t.remaining <= 0.0f) 
        {
			// コールバック登録
            if (t.cb) calls.push_back(t.cb);
           
			// インターバルかどうかで処理分岐
            if (t.interval > 0.0f) 
            {
				// インターバル: 残り時間をリセットして継続
                t.remaining += t.interval;
                ++it;
            }
            else 
            {
				// タイムアウト: タイマー削除
                it = timers_.erase(it);
            }
        } 
        else
        {
			// 継続
            ++it;
        }
    }

	// コールバック呼び出しループ
    for (auto& cb : calls) 
    {
		// 安全のため例外キャッチ
        try { cb(); } catch (...) {} 
    }
}

void TimeManager::Tick()
{
	// デルタタイム計算
    using clock = std::chrono::steady_clock;
    auto now = clock::now();
    float unscaledDt = 1.0f / 60.0f;

	// 初回更新時は前回時間が不定なのでスキップ
    if (initialized_) 
    {
        std::chrono::duration<float> elapsed = now - lastTime_;
        unscaledDt = elapsed.count();
        if (unscaledDt > 0.1f) unscaledDt = 0.1f; // clamp large dt (alt-tab)
    }
	// 更新時間保存   
    lastTime_ = now;
    unscaledDeltaTime_ = unscaledDt;

	// 停止中の処理
    if (paused_) 
    {
        if (stepAccumulator_ > 0.0f) 
        {
			// ステップ実行中: dtをステップ分だけに制限
            float use = std::min(stepAccumulator_, unscaledDt);
            stepAccumulator_ -= use;
            unscaledDt = use;
        } 
        else 
        {
			// 停止中: dtを0にしてタイマー更新のみ行う
            deltaTime_ = 0.0f;
            UpdateTimers(0.0f, unscaledDt);
            return;
        }
    }

	// スムーズなタイムスケール変更の更新
    if (smoothingEnabled_) 
    {
		// 経過時間を進める
        smoothElapsed_ += unscaledDt;
        float t = std::clamp(smoothElapsed_ / std::max(1e-6f, smoothDuration_), 0.0f, 1.0f);
        timeScale_ = smoothStart_ + (smoothTarget_ - smoothStart_) * t;
        if (t >= 1.0f) smoothingEnabled_ = false;
        for (auto& cb : timeScaleCallbacks_) if (cb) cb(timeScale_);
    }

	// スケールされたデルタタイム計算
    deltaTime_ = unscaledDt * timeScale_;

	// タイマー更新
    UpdateTimers(deltaTime_, unscaledDt);
}

void TimeManager::CancelSmoothTimeScale()
{
	// スムーズ変更キャンセル
    smoothingEnabled_ = false;
}

void TimeManager::Pause()
{
	// 時間停止
    paused_ = true;
}

void TimeManager::Resume()
{
	// 時間再開
    paused_ = false; 
    stepAccumulator_ = 0.0f;
}

void TimeManager::Step(float seconds)
{
    // 一時停止中のみ有効
    if (paused_)
    {
        // ステップ実行用に秒数を加算
        stepAccumulator_ += seconds;
    }
}

void TimeManager::CancelTimer(TimerId id)
{
	// タイマー検索ループ
    for (auto& t : timers_)
    {
		// 指定IDのタイマーを検索
        if (t.id == id) 
        {
			// タイマーをキャンセル状態に設定
            t.cancelled = true;
            break; 
        }
    }
}

void TimeManager::SmoothTimeScale(float toScale, float durationSeconds)
{
    // スムーズなタイムスケール変更の設定
    smoothStart_ = timeScale_;
    smoothTarget_ = std::max(0.0f, toScale);
    smoothDuration_ = std::max(0.0f, durationSeconds);
    smoothElapsed_ = 0.0f;
    smoothingEnabled_ = (smoothDuration_ > 0.0f);
	// 即時変更の場合は直接設定
    if (!smoothingEnabled_)
    {
        SetTimeScale(toScale);
    }
}

void TimeManager::AddTimeScaleCallback(TimeScaleCallback cb)
{
	// タイムスケール変更コールバックの追加
    timeScaleCallbacks_.push_back(std::move(cb));
}
