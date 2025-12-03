#include "SceneManager.h"
#include <cassert>

SceneManager* SceneManager::GetInstance()
{
    static SceneManager instance;
    return &instance;
}

void SceneManager::Finalize()
{
    scene_->Finalize();
    delete scene_;
}

void SceneManager::Update()
{
    if (nextScene_) {
        // 旧シーン終了
        if (scene_) {
            scene_->Finalize();
            delete scene_;
        }

        // シーン切り替え
        scene_ = nextScene_;
        nextScene_ = nullptr;

        // シーンマネージャをセット
        scene_->SetSceneManager(this);

        // 次のシーンを初期化
        scene_->Initialize();
    }

    // 実行中シーンを更新する
    scene_->Update();
}

void SceneManager::Draw()
{
    // 実行中シーンを描画する
    scene_->Draw();
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
    assert(sceneFactory_);
    assert(nextScene_ == nullptr);

    // 次のシーンを生成
    nextScene_ = sceneFactory_->CreateScene(sceneName);
}