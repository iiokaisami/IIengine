#pragma once

namespace IIEngine {class SceneManager;}

/// <summary>
/// シーンの基底クラス
/// 継承して使用する
/// </summary>
class BaseScene
{
public:
    // デストラクタ
    virtual ~BaseScene() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    virtual void Initialize();

    /// <summary>
    /// 終了
    /// </summary>
    virtual void Finalize();

    /// <summary>
    /// 更新処理
    /// </summary>
    virtual void Update();

    /// <summary>
    /// 描画処理
    /// </summary>
    virtual void Draw();

public:

    /// <summary>
	/// シーンマネージャのセット
    /// </summary>
    /// <param name="sceneManager">シーン管理を行うSceneManagerのポインタ</param>
    virtual void SetSceneManager(IIEngine::SceneManager* sceneManager) { sceneManager_ = sceneManager; }

protected:

    // シーンマネージャ
    IIEngine::SceneManager* sceneManager_ = nullptr;
};