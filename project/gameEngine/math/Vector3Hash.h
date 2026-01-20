#pragma once
#include <functional>
#include "Vector3.h"

namespace std
{
    template<>
    struct hash<Vector3>
    {
        size_t operator()(const Vector3& v) const noexcept
        {
            size_t h1 = std::hash<float>{}(v.x);
            size_t h2 = std::hash<float>{}(v.y);
            size_t h3 = std::hash<float>{}(v.z);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}
