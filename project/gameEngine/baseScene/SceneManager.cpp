#include "SceneManager.h"
#include <cassert>

SceneManager* SceneManager::GetInstance()
{
    static SceneManager instance;
    return &instance;
}

void SceneManager::Finalize()
{
    if (scene_) 
    {
        scene_->Finalize();
        scene_.reset();
    }
}

void SceneManager::Update()
{
    if (nextScene_)
    {
        // 旧シーン終了
        if (scene_)
        {
            scene_->Finalize();
            scene_.reset();
        }

        // シーン切り替え
        scene_ = std::move(nextScene_);
        nextScene_.reset();

        // シーンマネージャをセット
        scene_->SetSceneManager(this);

        // 次のシーンを初期化
        scene_->Initialize();
    }

    // 実行中シーンを更新する
    if (scene_) 
    {
        scene_->Update();
    }
}

void SceneManager::Draw()
{
    // 実行中シーンを描画する
    if (scene_)
    {
        scene_->Draw();
    }
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
    assert(sceneFactory_);
    assert(nextScene_ == nullptr);

    // 次のシーンを生成
    nextScene_ = sceneFactory_->CreateScene(sceneName);
}