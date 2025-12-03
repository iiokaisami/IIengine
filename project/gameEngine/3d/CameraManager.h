#pragma once

#include "Camera.h"
#include <vector>
#include <memory>

#include <limits>

#undef max
#undef min

/// <summary>
/// カメラマネージャー
/// 複数のカメラを管理し、アクティブなカメラを切り替えたり、全カメラの更新を行ったりする
/// </summary>
class CameraManager
{
public:
    // シングルトンパターンを使いたい場合
    static CameraManager& GetInstance() 
    {
        static CameraManager instance;
        return instance;
    }

    /// <summary>
    /// カメラの追加
    /// </summary>
    /// <param name="camera">追加するCameraの共有ポインタ</param>
    void AddCamera(std::shared_ptr<Camera> camera);

    /// <summary>
    /// カメラの削除
    /// シーンの変更時等に呼び出す
    /// </summary>
    /// <param name="index">削除するカメラのインデックス</param>
    void RemoveCamera(uint32_t index);

    /// <summary>
    /// アップデート(全カメラの更新処理を実行)
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void UpdateAll(float deltaTime = 0.0f);

    /// <summary>
    /// 特定のカメラのシェイクを開始
    /// </summary>
    /// <param name="index">シェイクを開始するカメラのインデックス</param>
    /// <param name="duration">シェイクの持続時間(秒)</param>
    /// <param name="magnitude">シェイクの強さ</param>
    void ShakeSpecificCamera(uint32_t index, float duration, float magnitude);

    // アクティブカメラのシェイクを開始
    // 軽い衝撃のシェイク
    // cameraManager.StartShakeActiveCamera(0.2f, 0.3f);
    // 中規模の爆発のシェイク
    // cameraManager.StartShakeActiveCamera(0.5f, 0.8f);
    // 大規模な爆発のシェイク
    // cameraManager.StartShakeActiveCamera(1.0f, 1.5f);

    /// <summary>
    /// アクティブカメラのシェイクを開始
    /// 軽い衝撃のシェイク
    /// cameraManager.StartShakeActiveCamera(0.2f, 0.3f);
    /// 中規模の爆発のシェイク
    /// cameraManager.StartShakeActiveCamera(0.5f, 0.8f);
    /// 大規模な爆発のシェイク
    /// cameraManager.StartShakeActiveCamera(1.0f, 1.5f);
    /// </summary>
    /// <param name="duration">シェイクの持続時間(秒)</param>
    /// <param name="magnitude">シェイクの強さ</param>
    void StartShakeActiveCamera(float duration, float magnitude);

    /// <summary>
    /// 全てのカメラを削除
    /// </summary>
    void ClearAllCameras();

public: // ゲッター

	// アクティブカメラのインデックス取得
    uint32_t GetActiveIndex()const { return activeCameraIndex_; }

    // アクティブカメラの取得
    std::shared_ptr<Camera> GetActiveCamera() const;

	// カメラの数を取得
	uint32_t GetCameraCount() const { return static_cast<uint32_t>(cameras_.size()); }

public: // セッター

    /// <summary>
    /// アクティブカメラを設定
    /// </summary>
    /// <param name="index">アクティブにしたいカメラのインデックス</param>
    void SetActiveCamera(uint32_t index);

private:
    CameraManager() = default;
    ~CameraManager() = default;

    CameraManager(const CameraManager&) = delete;
    CameraManager& operator=(const CameraManager&) = delete;

private:
    std::vector<std::shared_ptr<Camera>> cameras_;  // カメラのリスト
    uint32_t activeCameraIndex_ = ::std::numeric_limits<uint32_t>::max();  // アクティブなカメラのインデックス (無効値を設定)
};


