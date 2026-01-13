#pragma once

#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <unordered_map>

#include <Model.h>
#include <Object3d.h>

#include "../../externals/json/json.hpp"

namespace IIEngine
{

	/// <summary>
	/// レベルデータ
	/// レベルエディタで作成したデータを読み込むためのクラス
	/// </summary>
	class LevelData
	{
	public:

		// プレイヤーのスポーンデータ
		struct PlayerSpawnData
		{
			Vector3 position;
			Vector3 rotation;
		};

		// 敵キャラのスポーンデータ
		struct EnemySpawnData
		{
			Vector3 position;
			Vector3 rotation;
			uint32_t waveNum = 1;
			uint32_t spawnDelay = 0;
			bool isSpawned = false; // 敵がスポーンしたかどうか
		};

		// 壁のデータ
		struct WallData
		{
			Vector3 position;
			Vector3 rotation;
			Vector3 scale;
		};

		// オブジェクトデータ
		struct ObjectData
		{
			std::string type;
			std::string fileName;
			Vector3 translation;
			Vector3 rotation;
			Vector3 scale;
		};

		// オブジェクト配列
		std::vector<ObjectData> objects;


		// プレイヤー配列
		std::vector<PlayerSpawnData> players;

		// 敵キャラ配列
		std::vector<EnemySpawnData> enemies;
		std::vector<EnemySpawnData> trapEnemies;

		// 壁配列
		std::vector<WallData> walls;

	};

	/// <summary>
	/// レベルデータローダー
	/// レベルエディタで作成したデータを読み込むためのクラス
	/// </summary>
	class LevelDataLoader
	{
	public:

		static constexpr const char* kDefaultBaseDirectory = "resources/levels/";
		static constexpr const char* kExtension = ".json";

		/// <summary>
		/// レベルデータの読み込み
		/// </summary>
		/// <param name="fileName">ファイル名（拡張子不要）</param>
		/// <returns>レベルデータのポインタ</returns>
		static std::unique_ptr<LevelData> LoadLevelData(const std::string& fileName);

	private:

		/// <summary>
		/// オブジェクトの再帰的読み込み
		/// </summary>
		/// <param name="objectJson">オブジェクトのJSONデータ</param>
		/// <param name="levelData">レベルデータのポインタ</param>
		static void LoadObjectRecursive(const nlohmann::json& objectJson, LevelData* levelData);

	};
}