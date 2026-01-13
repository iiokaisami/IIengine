#include "LevelDataLoader.h"

namespace IIEngine
{

    std::unique_ptr<LevelData> LevelDataLoader::LoadLevelData(const std::string& fileName)
    {
        std::string fullPath = kDefaultBaseDirectory + fileName + kExtension;

        std::ifstream file(fullPath);
        if (file.fail())
        {
            assert(false && "Failed to open level data file.");
            return nullptr;
        }

        nlohmann::json deserialized;
        file >> deserialized;

        assert(deserialized.is_object());
        assert(deserialized.contains("name") && deserialized["name"].is_string());
        assert(deserialized["name"] == "scene");

        auto levelData = std::make_unique<LevelData>();

        for (const auto& object : deserialized["objects"])
        {
            LoadObjectRecursive(object, levelData.get());
        }

        return levelData;
    }

    void LevelDataLoader::LoadObjectRecursive(const nlohmann::json& objectJson, LevelData* levelData)
    {
        assert(objectJson.contains("type") && objectJson["type"].is_string());

        if (objectJson.contains("disable_export"))
        {
            // 有効無効フラグ
            bool disabled = objectJson["disable_export"].get<bool>();
            if (!disabled)
            {
                // このオブジェクトは配置しない
                return;
            }
        }

        std::string type = objectJson["type"];

        if (type == "MESH")
        {
            LevelData::ObjectData data;
            data.type = type;

            if (objectJson.contains("file_name"))
            {
                std::string fileName = objectJson["file_name"];
                // すでに .obj で終わっていなければ付与
                // 4文字以下の場合 or 最後の4文字が ".obj" でない場合
                if (fileName.length() < 4 or fileName.substr(fileName.length() - 4) != ".obj")
                {
                    fileName += ".obj";
                }
                data.fileName = objectJson["file_name"];
            }

            if (objectJson.contains("transform"))
            {
                const auto& t = objectJson["transform"];
                data.translation = { -1.0f * t["translation"][0], t["translation"][1], -1.0f * t["translation"][2] };
                data.rotation = { -1.0f * t["rotation"][0],  3.14f + t["rotation"][1], -1.0f * t["rotation"][2] };
                data.scale = { t["scaling"][0], t["scaling"][1], t["scaling"][2] };
            }

            levelData->objects.emplace_back(std::move(data));
        }
        // プレイヤー発生ポイント
        else if (type == "PlayerSpawn")
        {
            // players に要素を 1 つ追加
            LevelData::PlayerSpawnData playerData;

            // transform 情報が存在すれば位置と回転を取得
            if (objectJson.contains("transform"))
            {
                const auto& t = objectJson["transform"];

                // position の数値を書き込む
                if (t.contains("translation"))
                {
                    const auto& pos = t["translation"];
                    playerData.position = { -1.0f * pos[0], pos[2], -1.0f * pos[1] };
                }

                // rotation の数値を書き込む（Blenderとの座標系補正が必要なら -1.0f を掛ける）
                if (t.contains("rotation"))
                {
                    const auto& rot = t["rotation"];
                    playerData.rotation = { -1.0f * rot[0], 3.14f + rot[1], -1.0f * rot[2] };
                }
            }

            levelData->players.push_back(playerData);
        }
        // 敵キャラ発生ポイント
        else if (type == "EnemySpawn")
        {
            // enemies に要素を 1 つ追加
            LevelData::EnemySpawnData enemyData;
            // transform 情報が存在すれば情報を取得
            if (objectJson.contains("transform"))
            {
                const auto& t = objectJson["transform"];
                // position の数値を書き込む
                if (t.contains("translation"))
                {
                    const auto& pos = t["translation"];
                    enemyData.position = { -1.0f * pos[0], pos[2], -1.0f * pos[1] };
                }
                // rotation の数値を書き込む（Blenderとの座標系補正が必要なら -1.0f を掛ける）
                if (t.contains("rotation"))
                {
                    const auto& rot = t["rotation"];
                    enemyData.rotation = { -1.0f * rot[0], 3.14f + rot[1], -1.0f * rot[2] };
                }
            }

            // wave_id と delay は transform の外にあるので objectJson から読む
            if (objectJson.contains("wave_id"))
            {
                enemyData.waveNum = objectJson["wave_id"].get<uint32_t>();
            }
            if (objectJson.contains("delay"))
            {
                enemyData.spawnDelay = objectJson["delay"].get<uint32_t>();
            }

            levelData->enemies.push_back(enemyData);
        }
        // 罠敵発生ポイント
        else if (type == "Trap")
        {
            // enemies に要素を 1 つ追加
            LevelData::EnemySpawnData enemyData;
            // transform 情報が存在すれば情報を取得
            if (objectJson.contains("transform"))
            {
                const auto& t = objectJson["transform"];
                // position の数値を書き込む
                if (t.contains("translation"))
                {
                    const auto& pos = t["translation"];
                    enemyData.position = { -1.0f * pos[0], pos[2], -1.0f * pos[1] };
                }
                // rotation の数値を書き込む（Blenderとの座標系補正が必要なら -1.0f を掛ける）
                if (t.contains("rotation"))
                {
                    const auto& rot = t["rotation"];
                    enemyData.rotation = { -1.0f * rot[0], 3.14f + rot[1], -1.0f * rot[2] };
                }
            }

            // wave_id と delay は transform の外にあるので objectJson から読む
            if (objectJson.contains("wave_id"))
            {
                enemyData.waveNum = objectJson["wave_id"].get<uint32_t>();
            }
            if (objectJson.contains("delay"))
            {
                enemyData.spawnDelay = objectJson["delay"].get<uint32_t>();
            }

            levelData->trapEnemies.push_back(enemyData);
        }
        // 壁発生ポイント
        else if (type == "Wall")
        {
            // walls に要素を 1 つ追加
            LevelData::WallData wallData;

            // transform 情報が存在すれば位置と回転を取得
            if (objectJson.contains("transform"))
            {
                const auto& t = objectJson["transform"];

                // position の数値を書き込む
                if (t.contains("translation"))
                {
                    const auto& pos = t["translation"];
                    wallData.position = { -1.0f * pos[0], pos[2], -1.0f * pos[1] };
                }

                // rotation の数値を書き込む（Blenderとの座標系補正が必要なら -1.0f を掛ける）
                if (t.contains("rotation"))
                {
                    const auto& rot = t["rotation"];
                    wallData.rotation = { -1.0f * rot[0], 3.14f + rot[1], -1.0f * rot[2] };
                }

                // scale の数値を書き込む
                if (t.contains("scaling"))
                {
                    const auto& scl = t["scaling"];
                    wallData.scale = { scl[0], scl[2], scl[1] };
                }

            }

            levelData->walls.push_back(wallData);

        }

        // 再帰 子オブジェクトの走査
        if (objectJson.contains("children"))
        {
            for (const auto& child : objectJson["children"])
            {
                LoadObjectRecursive(child, levelData);
            }
        }
    }
}