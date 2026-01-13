#pragma once

#include <string>

#include "MyMath.h"

namespace IIEngine
{

    /// <summary>
    /// パーティクル1つ分の情報
    /// 粒子の位置、速度、寿命など
    /// </summary>
    class Particle
    {
    public:
        struct Transform
        {
            Vector3 scale;
            Vector3 rotate;
            Vector3 translate;
        };

        // パーティクルの状態
        Transform transform;
        Vector3 velocity;
        Vector3 angularVelocity{ 0,0,0 };
        Vector3 scaleVelocity{ 0,0,0 };
        Vector4 color;
        float lifeTime;
        float currentTime;

        std::string motionName;

        // コンストラクタ
        Particle()
            : velocity(0.0f, 0.0f, 0.0f), color(1.0f, 1.0f, 1.0f, 1.0f), lifeTime(1.0f), currentTime(0.0f)
        {
            transform.scale = Vector3(1.0f, 1.0f, 1.0f);
            transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
            transform.translate = Vector3(0.0f, 0.0f, 0.0f);
        }
    };

}