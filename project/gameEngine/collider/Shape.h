#pragma once

#include "Vector3.h"

/// <summary>
/// 形状の種類
/// </summary>
enum class Shape
{
    Sphere,
    OBB,
    AABB,
};

struct AABB
{
    Vector3 min; // 最小点
    Vector3 max; // 最大点
};

struct OBB
{
    Vector3 center; // 中心点
    Vector3 orientations[3]; // 座標軸。正規化・直交必須
    Vector3 size; // 座標軸方向の長さの半分。中心から面までの距離
};

struct Sphere
{
    Vector3 center; // 中心点
    float radius; // 半径
};