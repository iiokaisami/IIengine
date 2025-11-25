#pragma once

#include <unordered_map>
#include <functional>
#include <string>
#include <random>
#include <MyMath.h>

#include "Particle.h"

/// <summary>
/// パーティクルのモーション管理
/// 登録、生成、一覧取得など
/// </summary>
class ParticleMotion
{
public:

    using MotionFunc = std::function<Particle(std::mt19937&, const Vector3&)>;

    /// <summary>
	/// モーションの登録
    /// </summary>
	/// <param name="name">モーション名</param>
	/// <param name="func">モーション生成関数</param>
    static void Register(const std::string& name, MotionFunc func);

	/// <summary>
	/// パーティクルの生成
	/// </summary>
	/// <param name="name">モーション名</param>
	/// <param name="rand">乱数生成器</param>
	/// <param name="pos">生成位置</param>
	/// <returns>生成したパーティクル</returns>
    static Particle Create(const std::string& name, std::mt19937& rand, const Vector3& pos);

    /// <summary>
	/// 初期化(モーション登録)
    /// </summary>
    static void Initialize();

    /// <summary>
    /// 登録済み一覧取得(ImGui などUI表示用)
    /// </summary>
	/// <returns>モーション関数のマップ</returns>
    static const std::unordered_map<std::string, MotionFunc>& GetAll();

    // 各モーション関数の定義
 
    /// <summary>
	/// ターゲットに向かって飛ぶ
    /// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="target">目標位置</param>
	/// <returns>生成したパーティクル</returns>
    static Particle MakeHoming(std::mt19937& rand, const Vector3& target);
    /// <summary>
	/// ターゲットの周りを回る
    /// </summary>
	/// <param name="rand">ランダム生成器</param>
    /// <param name="target">目標位置</param>
    /// <returns>生成したパーティクル</returns>
    static Particle MakeOrbit(std::mt19937& rand, const Vector3& target);
	/// <summary>
	/// 中心からランダムに放射状に飛ぶ
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="center">中心</param>
	/// <returns>生成したパーティクル</returns>
    static Particle MakeExplosion(std::mt19937& rand, const Vector3& center);
	/// <summary>
	/// 中心から破裂するように飛ぶ
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="center">中心</param>
	/// <returns>生成したパーティクル</returns>
	static Particle MakeRupture(std::mt19937& rand, const Vector3& center);
	/// <summary>
	/// baseから上に向かって噴出
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="base">発現地点</param>
	/// <returns>生成したパーティクル</returns>
    static Particle MakeFountain(std::mt19937& rand, const Vector3& base);
	/// <summary>
	/// originを中心にランダムに揺れる
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="origin">中心</param>
	/// <returns>生成したパーティクル</returns>
    static Particle MakeWiggle(std::mt19937& rand, const Vector3& origin);
	/// <summary>
	/// 円柱状に放射
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="position">発現位置</param>
	/// <returns>生成したパーティクル</returns>
    static Particle MakeCylinder(std::mt19937& rand, const Vector3& position);
	/// <summary>
	/// 斜めにスラッシュ
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="translate">発現地点</param>
	/// <returns>生成したパーティクル</returns>
    static Particle MakeSlash(std::mt19937& rand, const Vector3& translate);
	/// <summary>
	/// 炎のように上昇
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="base">発現位置</param>
	/// <returns>生成したパーティクル</returns>
    static Particle MakeFlame(std::mt19937& rand, const Vector3& base);

    // 魔法陣
	/// <summary>
	/// 魔法陣
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="translate">発現地点</param>
	/// <returns>生成したパーティクル</returns>
    static Particle MakeMagic1(std::mt19937& rand, const Vector3& translate);
	/// <summary>
	/// 魔法陣パラパラ
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="translate">中心</param>
	/// <returns>生成したパーティクル</returns>
	static Particle MakeMagic2(std::mt19937& rand, const Vector3& translate);
	/// <summary>
	/// レーザー
	/// </summary>
	/// <param name="rand">ランダム生成期</param>
	/// <param name="translate">発現地点</param>
	/// <returns>生成したパーティクル</returns>
	static Particle MakeLaser(std::mt19937& rand, const Vector3& translate);

	/// <summary>
	/// 花びら
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="base">発現位置</param>
	/// <returns>生成したパーティクル</returns>
	static Particle MakePetal(std::mt19937& rand, const Vector3& translate);

	/// <summary>
	/// 水
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="base">発現位置</param>
	/// <returns>生成したパーティクル</returns>
	static Particle MakeWater(std::mt19937& rand, const Vector3& translate);

	/// <summary>
	/// 泡
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="base">発現位置</param>
	/// <returns>生成したパーティクル</returns>
	static Particle MakeBubble(std::mt19937& rand, const Vector3& translate);

	/// <summary>
	/// 土埃
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="base">発現位置</param>
	/// <returns>生成したパーティクル</returns>
	static Particle MakeDust(std::mt19937& rand, const Vector3& translate);

	/// <summary>
	/// 敵土埃
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="base">発現位置</param>
	/// <returns>生成したパーティクル</returns>
	static Particle MakeEnemyDust(std::mt19937& rand, const Vector3& translate);
    
	/// <summary>
	/// デバフ
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="base">発現位置</param>
	/// <returns>生成したパーティクル</returns>
	static Particle MakeDebuff(std::mt19937& rand, const Vector3& translate);

	/// <summary>
	/// スパーク
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="base">発現位置</param>
	/// <returns>生成したパーティクル</returns>
	static Particle MakeSpark(std::mt19937& rand, const Vector3& translate);

	/// <summary>
	/// 弾けるスパーク
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="base">発現位置</param>
	/// <returns>生成したパーティクル</returns>
	static Particle MakeSparkBurst(std::mt19937& rand, const Vector3& translate);
 
	/// <summary>
	/// ヒットリアクション中心からサイドに弾けるように飛ぶ
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="translate">発現位置</param>
	/// <returns>生成したパーティクル</returns>
	static Particle MakeHitReaction(std::mt19937& rand, const Vector3& translate);

	/// <summary>
	/// 弾丸ヒットリアクション
	/// </summary>
	/// <param name="rand">ランダム生成器</param>
	/// <param name="translate">発現位置</param>
	/// <returns>生成したパーティクル</returns>
	static Particle MakeBulletHitReaction(std::mt19937& rand, const Vector3& translate);

public: // セッター

	/// <summary>
	/// Ring方向設定
	/// "UP"下から上
	/// "DOWN"上から下
	/// "LEFT"右から左
	/// "RIGHT"左から右
	/// </summary>
	/// <param name="direction">方向文字列</param>
    static void SetDirection(const std::string& direction);

private:

    static std::unordered_map<std::string, MotionFunc> motions_;

    static std::string direction_;

};

