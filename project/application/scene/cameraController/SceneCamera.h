#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <Vector3.h>

#include "ICameraController.h"

/// 前方宣言
class Camera;
class CameraManager;

/// <summary>
/// シーンカメラ
/// </summary>
class SceneCamera
{
public:

	// コンストラクタ
	SceneCamera();
	// デストラクタ
	~SceneCamera();

	// 初期化
	void Initialize();
	// 終了
	void Finalize();

	/// <summary>
	/// コントローラー追加
	/// </summary>
	/// <param name="controller">追加するコントローラーのユニークポインタ</param>
	void AddController(std::unique_ptr<ICameraController> controller);
	// カメラ更新
	void Update(float dt);

public: // ゲッター

	// カメラ取得
	std::shared_ptr<Camera> GetCamera() const { return camera_; }

	// 位置取得
	Vector3 GetPosition() const;
	// 回転取得
	Vector3 GetRotate() const;
	
public: // セッター

	// 位置設定
	void SetPosition(const Vector3& position);
	// 回転設定
	void SetRotate(const Vector3& rotate);

private:

	// カメラ
	std::shared_ptr<Camera> camera_ = nullptr;
	// カメラコントローラーリスト
	std::vector<std::unique_ptr<ICameraController>> controllers_;
	// カメラマネージャー
	CameraManager& cameraManager_;

};

