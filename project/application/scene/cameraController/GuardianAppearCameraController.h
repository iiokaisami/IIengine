#pragma once

#include <memory>
#include <functional>
#include <algorithm>

#include <Vector3.h>

#include "ICameraController.h"
#include "../../../gameEngine/3d/Camera.h"

/// <summary>
/// ガーディアン出現カメラコントローラー
/// </summary>
class GuardianAppearCameraController : public ICameraController
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="getGuardianPos">ガーディアンの位置を取得する関数</param>
	/// <param name="getFollowCameraPos">フォローカメラの位置を取得する関数</param>
	/// <param name="moveDuration">移動時間</param>
	/// <param name="waitDuration">待機時間</param>
	/// <param name="returnDuration">戻る時間</param>
	GuardianAppearCameraController(	std::shared_ptr<IIEngine::Camera> camera,const std::function<Vector3()>& getGuardianPos,const std::function<Vector3()>& getFollowCameraPos,float moveDuration = 0.8f,float waitDuration = 1.2f,float returnDuration = 0.6f);

	// デストラクタ
	~GuardianAppearCameraController() override = default;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt">経過時間</param>
	void Update(float dt) override;

    void Start() override;
    void Stop() override
    {
        active_ = false;
    }

    bool IsActive() const override
    {
        return active_;
    }

public:

    void SetOnFinish(std::function<void()> cb)
    {
        onFinish_ = std::move(cb);
    }

private:

    enum class Phase
    {
        None,
        MoveToGuardian,
        Wait,
        Return,
        Finished
    };

private:

    std::shared_ptr<IIEngine::Camera> camera_;

    std::function<Vector3()> getGuardianPos_;
    std::function<Vector3()> getFollowCameraPos_;

    bool active_ = false;

    Phase phase_ = Phase::None;

    float timer_ = 0.0f;


    float moveDuration_ = 0.7f;
    float waitDuration_ = 0.8f;
    float returnDuration_ = 0.6f;

    Vector3 guardianOffset_ =
    {
        0.0f,
        8.0f,
        -12.0f
    };

    Vector3 fromPos_;
    Vector3 toPos_;

    Vector3 fromRot_;
    Vector3 toRot_;

    std::function<void()> onFinish_;

    Vector3 guardianPos_{};
    Vector3 returnCameraPos_{};

    Vector3 phaseFromPos_{};
    Vector3 phaseToPos_{};

    Vector3 arrivedPos_{};
    Vector3 arrivedRot_{};

    Vector3 startRot_{};
    Vector3 returnStartRot_{};
    Vector3 returnTargetRot_{};

};