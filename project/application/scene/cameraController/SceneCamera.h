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

	// 指定したコントローラTを生成登録、生ポインタを返す
	template<typename T, typename... Args>
	T* AddController(Args&&... args)
	{
		static_assert(std::is_base_of<ICameraController, T>::value, "T must derive from ICameraController");
		auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
		T* raw = ptr.get();
		controllers_.push_back(std::move(ptr));
		return raw;
	}

	// 登録済みのコントローラTを検索してT*を返す
	template<typename T>
	T* FindController() const
	{
		static_assert(std::is_base_of<ICameraController, T>::value, "T must derive from ICameraController");
		for (const auto& c : controllers_)
		{
			if (auto p = dynamic_cast<T*>(c.get())) return p;
		}
		return nullptr;
	}
	
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

