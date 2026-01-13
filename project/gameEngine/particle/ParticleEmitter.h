#pragma once

#include <unordered_map>
#include <random> 

#include "ParticleManager.h"

namespace IIEngine
{

	/// <summary>
	/// パーティクルエミッター
	/// パーティクルを発生させる
	/// </summary>
	class ParticleEmitter
	{
	public:

		/// <summary>
		/// 静的単発エミット
		/// </summary>
		/// <param name="groupName">グループ名</param>
		/// <param name="position">発生位置</param>
		/// <param name="count">発生数</param>
		static void Emit(const std::string& groupName, const Vector3& position, uint32_t count = 10);

		/// <summary>
		/// 静的ループエミット
		/// </summary>
		/// <param name="groupName">グループ名</param>
		/// <param name="motionName">モーション名</param>
		/// <param name="position">発生位置</param>
		/// <param name="count">発生数</param>
		static void StartLoop(const std::string& groupName, const std::string& motionName, const Vector3& position, uint32_t count = 10);

		// --- 以下はインスタンス用：必要なら使う ---
		// コンストラクタ
		ParticleEmitter(ParticleManager* manager, const std::string& groupName, const std::string& motionName = "Homing")
			: manager_(manager), groupName_(groupName), motionName_(motionName)
		{
		}

		/// <summary>
		/// 単発エミット
		/// </summary>
		/// <param name="position">発生位置</param>
		/// <param name="count">発生数</param>
		void EmitOnce(const Vector3& position, uint32_t count = 10);

		/// <summary>
		/// ループエミット開始
		/// </summary>
		/// <param name="position">発生位置</param>
		/// <param name="count">発生数</param>
		void StartLoopEmit(const Vector3& position, uint32_t count);


	private:

		// Managerクラス
		ParticleManager* manager_ = nullptr;

		// グループ名
		std::string groupName_;

		// モーション名
		std::string motionName_;

	};

}