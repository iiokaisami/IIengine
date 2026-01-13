#include "MeshBuilder.h"

#include <numbers>

namespace IIEngine
{

    void MeshBuilder::BuildRing(Model* model)
    {
        model->ClearVertexData();

        const uint32_t kRingDivide = 32;
        const float kOuterRadius = 1.0f;
        const float kInnerRadius = 0.2f;
        const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kRingDivide);

        for (uint32_t index = 0; index < kRingDivide; ++index)
        {
            float sin = std::sin(index * radianPerDivide);
            float cos = std::cos(index * radianPerDivide);
            float sinNext = std::sin((index + 1) * radianPerDivide);
            float cosNext = std::cos((index + 1) * radianPerDivide);
            float u = float(index) / float(kRingDivide);
            float uNext = float(index + 1) / float(kRingDivide);

            uint32_t baseIndex = model->GetVertexCount();

            model->AddVertex({ -sin * kOuterRadius, cos * kOuterRadius, 0.0f, 1.0f }, { u, 0.0f }, { 0.0f, 0.0f, 1.0f });
            model->AddVertex({ -sin * kInnerRadius, cos * kInnerRadius, 0.0f, 1.0f }, { u, 1.0f }, { 0.0f, 0.0f, 1.0f });
            model->AddVertex({ -sinNext * kOuterRadius, cosNext * kOuterRadius, 0.0f, 1.0f }, { uNext, 0.0f }, { 0.0f, 0.0f, 1.0f });

            model->AddIndex(baseIndex + 0);
            model->AddIndex(baseIndex + 1);
            model->AddIndex(baseIndex + 2);

            model->AddVertex({ -sinNext * kOuterRadius, cosNext * kOuterRadius, 0.0f, 1.0f }, { uNext, 0.0f }, { 0.0f, 0.0f, 1.0f });
            model->AddVertex({ -sin * kInnerRadius, cos * kInnerRadius, 0.0f, 1.0f }, { u, 1.0f }, { 0.0f, 0.0f, 1.0f });
            model->AddVertex({ -sinNext * kInnerRadius, cosNext * kInnerRadius, 0.0f, 1.0f }, { uNext, 1.0f }, { 0.0f, 0.0f, 1.0f });

            model->AddIndex(baseIndex + 3);
            model->AddIndex(baseIndex + 4);
            model->AddIndex(baseIndex + 5);
        }

        model->UpdateVertexBuffer();
        model->UpdateIndexBuffer();
    }

    void MeshBuilder::BuildCylinder(Model* model)
    {
        model->ClearVertexData();

        const uint32_t kDivide = 32;
        const float kRadius = 1.0f;
        const float kHeight = 3.0f;
        const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kDivide);

        for (uint32_t i = 0; i < kDivide; ++i)
        {
            float sin = std::sin(i * radianPerDivide);
            float cos = std::cos(i * radianPerDivide);
            float sinNext = std::sin((i + 1) * radianPerDivide);
            float cosNext = std::cos((i + 1) * radianPerDivide);
            float u = float(i) / float(kDivide);
            float uNext = float(i + 1) / float(kDivide);

            Vector3 top1 = { -sin * kRadius, kHeight, cos * kRadius };
            Vector3 top2 = { -sinNext * kRadius, kHeight, cosNext * kRadius };
            Vector3 bottom1 = { -sin * kRadius, 0.0f, cos * kRadius };
            Vector3 bottom2 = { -sinNext * kRadius, 0.0f, cosNext * kRadius };

            uint32_t baseIndex = model->GetVertexCount();

            model->AddVertex({ top1.x, top1.y, top1.z, 1.0f }, { u, 0.0f }, { -sin, 0.0f, cos });
            model->AddVertex({ top2.x, top2.y, top2.z, 1.0f }, { uNext, 0.0f }, { -sinNext, 0.0f, cosNext });
            model->AddVertex({ bottom1.x, bottom1.y, bottom1.z, 1.0f }, { u, 1.0f }, { -sin, 0.0f, cos });

            model->AddVertex({ bottom1.x, bottom1.y, bottom1.z, 1.0f }, { u, 1.0f }, { -sin, 0.0f, cos });
            model->AddVertex({ top2.x, top2.y, top2.z, 1.0f }, { uNext, 0.0f }, { -sinNext, 0.0f, cosNext });
            model->AddVertex({ bottom2.x, bottom2.y, bottom2.z, 1.0f }, { uNext, 1.0f }, { -sinNext, 0.0f, cosNext });

            model->AddIndex(baseIndex + 0);
            model->AddIndex(baseIndex + 1);
            model->AddIndex(baseIndex + 2);
            model->AddIndex(baseIndex + 3);
            model->AddIndex(baseIndex + 4);
            model->AddIndex(baseIndex + 5);
        }

        model->UpdateVertexBuffer();
        model->UpdateIndexBuffer();
    }

    void MeshBuilder::BuildCone(Model* model)
    {
        model->ClearVertexData();

        const uint32_t kDivide = 32;
        const float kRadius = 1.0f;
        const float kHeight = 3.0f;
        const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / float(kDivide);

        Vector3 apex = { 0.0f, +kHeight * 0.5f, 0.0f };     // 頂点：上に1.5
        uint32_t apexIndex = model->GetVertexCount();
        model->AddVertex({ apex.x, apex.y, apex.z, 1.0f }, { 0.5f, 0.0f }, { 0, 1, 0 });

        for (uint32_t i = 0; i < kDivide; ++i)
        {
            float sin = std::sin(i * radianPerDivide);
            float cos = std::cos(i * radianPerDivide);
            float sinNext = std::sin((i + 1) * radianPerDivide);
            float cosNext = std::cos((i + 1) * radianPerDivide);


            Vector3 base1 = { -sin * kRadius, -kHeight * 0.5f, cos * kRadius };
            Vector3 base2 = { -sinNext * kRadius, 0.0f, cosNext * kRadius };

            uint32_t base = model->GetVertexCount();
            model->AddVertex({ base1.x, base1.y, base1.z, 1.0f }, { 0.0f, 1.0f }, { -sin, 0, cos });
            model->AddVertex({ base2.x, base2.y, base2.z, 1.0f }, { 1.0f, 1.0f }, { -sinNext, 0, cosNext });

            model->AddIndex(apexIndex);
            model->AddIndex(base);
            model->AddIndex(base + 1);
        }

        model->UpdateVertexBuffer();
        model->UpdateIndexBuffer();
    }

    void MeshBuilder::BuildSpiral(Model* model)
    {
        model->ClearVertexData();

        const uint32_t kDivisions = 50;       // 分割数
        const float kWidth = 1.0f;            // 長方形の幅（X方向）
        const float kHeight = 5.0f;           // 長方形の高さ（Z方向）
        const float kTwist = 3.0f * std::numbers::pi_v<float>; // ねじれ量（ラジアン）

        for (uint32_t i = 0; i <= kDivisions; ++i)
        {
            float t = float(i) / float(kDivisions);
            float z = t * kHeight;
            float angle = t * kTwist;

            // 回転に応じたX, Y 位置（幅の左右）
            float xLeft = -std::cos(angle) * (kWidth * 0.5f);
            float yLeft = -std::sin(angle) * (kWidth * 0.5f);
            float xRight = std::cos(angle) * (kWidth * 0.5f);
            float yRight = std::sin(angle) * (kWidth * 0.5f);

            Vector3 normal = { 0.0f, 0.0f, 1.0f }; // 単純にZ方向

            // 左右の2頂点を追加
            model->AddVertex({ xLeft,  yLeft,  z, 1.0f }, { 0.0f, t }, normal);
            model->AddVertex({ xRight, yRight, z, 1.0f }, { 1.0f, t }, normal);
        }

        // インデックス（三角形化）
        for (uint32_t i = 0; i < kDivisions; ++i)
        {
            uint32_t base = i * 2;
            model->AddIndex(base + 0);
            model->AddIndex(base + 1);
            model->AddIndex(base + 2);

            model->AddIndex(base + 2);
            model->AddIndex(base + 1);
            model->AddIndex(base + 3);
        }

        model->UpdateVertexBuffer();
        model->UpdateIndexBuffer();
    }

    void MeshBuilder::BuildTorus(Model* model)
    {
        model->ClearVertexData();

        const uint32_t kTubeDiv = 16;
        const uint32_t kCircleDiv = 32;
        const float kOuterR = 1.5f;
        const float kInnerR = 0.4f;

        for (uint32_t i = 0; i < kCircleDiv; ++i)
        {
            float theta = 2.0f * std::numbers::pi_v<float> *float(i) / float(kCircleDiv);
            float nextTheta = 2.0f * std::numbers::pi_v<float> *float(i + 1) / float(kCircleDiv);

            for (uint32_t j = 0; j < kTubeDiv; ++j)
            {
                float phi = 2.0f * std::numbers::pi_v<float> *float(j) / float(kTubeDiv);
                float nextPhi = 2.0f * std::numbers::pi_v<float> *float(j + 1) / float(kTubeDiv);

                auto point = [&](float t, float p) -> Vector3
                    {
                        float x = (kOuterR + kInnerR * std::cos(p)) * std::cos(t);
                        float y = kInnerR * std::sin(p);
                        float z = (kOuterR + kInnerR * std::cos(p)) * std::sin(t);
                        return { x, y, z };
                    };

                Vector3 v0 = point(theta, phi);
                Vector3 v1 = point(nextTheta, phi);
                Vector3 v2 = point(theta, nextPhi);
                Vector3 v3 = point(nextTheta, nextPhi);

                uint32_t base = model->GetVertexCount();
                model->AddVertex({ v0.x, v0.y, v0.z, 1.0f }, { 0,0 }, { 0,1,0 });
                model->AddVertex({ v1.x, v1.y, v1.z, 1.0f }, { 1,0 }, { 0,1,0 });
                model->AddVertex({ v2.x, v2.y, v2.z, 1.0f }, { 0,1 }, { 0,1,0 });
                model->AddVertex({ v3.x, v3.y, v3.z, 1.0f }, { 1,1 }, { 0,1,0 });

                model->AddIndex(base + 0);
                model->AddIndex(base + 1);
                model->AddIndex(base + 2);
                model->AddIndex(base + 2);
                model->AddIndex(base + 1);
                model->AddIndex(base + 3);
            }
        }

        model->UpdateVertexBuffer();
        model->UpdateIndexBuffer();
    }

    void MeshBuilder::BuildHelix(Model* model)
    {
        model->ClearVertexData();

        const uint32_t kSegments = 100;
        const float kRadius = 1.0f;
        const float kHeight = 5.0f;
        const uint32_t kTurns = 5;
        const float kWidth = 0.05f;

        for (uint32_t i = 0; i < kSegments; ++i)
        {
            float t = float(i) / float(kSegments);
            float angle = t * kTurns * 2.0f * std::numbers::pi_v<float>;

            float x = std::cos(angle) * kRadius;
            float y = t * kHeight;
            float z = std::sin(angle) * kRadius;

            float nextT = float(i + 1) / float(kSegments);
            float nextAngle = nextT * kTurns * 2.0f * std::numbers::pi_v<float>;
            float nx = std::cos(nextAngle) * kRadius;
            float ny = nextT * kHeight;
            float nz = std::sin(nextAngle) * kRadius;

            Vector3 pos1 = { x, y, z };
            Vector3 pos2 = { nx, ny, nz };
            Vector3 dir = Normalize(pos2 - pos1);
            Vector3 right = Normalize(Cross({ 0, 1, 0 }, dir)) * kWidth;

            Vector3 v0 = pos1 + right;
            Vector3 v1 = pos1 - right;
            Vector3 v2 = pos2 + right;
            Vector3 v3 = pos2 - right;

            uint32_t base = model->GetVertexCount();
            model->AddVertex({ v0.x, v0.y, v0.z, 1.0f }, { 0,0 }, { 0,1,0 });
            model->AddVertex({ v1.x, v1.y, v1.z, 1.0f }, { 1,0 }, { 0,1,0 });
            model->AddVertex({ v2.x, v2.y, v2.z, 1.0f }, { 0,1 }, { 0,1,0 });
            model->AddVertex({ v3.x, v3.y, v3.z, 1.0f }, { 1,1 }, { 0,1,0 });

            model->AddIndex(base + 0);
            model->AddIndex(base + 1);
            model->AddIndex(base + 2);

            model->AddIndex(base + 2);
            model->AddIndex(base + 1);
            model->AddIndex(base + 3);
        }

        model->UpdateVertexBuffer();
        model->UpdateIndexBuffer();
    }

    void MeshBuilder::BuildSphere(Model* model)
    {
        model->ClearVertexData();

        const uint32_t kLatitude = 16;   // 緯度分割数
        const uint32_t kLongitude = 32;  // 経度分割数
        const float kRadius = 0.1f;

        for (uint32_t lat = 0; lat <= kLatitude; ++lat)
        {
            float theta = float(lat) * std::numbers::pi_v<float> / float(kLatitude);
            float y = std::cos(theta) * kRadius;
            float r = std::sin(theta) * kRadius;
            float v = float(lat) / float(kLatitude);

            for (uint32_t lon = 0; lon <= kLongitude; ++lon)
            {
                float phi = float(lon) * 2.0f * std::numbers::pi_v<float> / float(kLongitude);
                float x = std::cos(phi) * r;
                float z = std::sin(phi) * r;
                float u = float(lon) / float(kLongitude);

                Vector3 normal = { x / kRadius, y / kRadius, z / kRadius };
                model->AddVertex({ x, y, z, 1.0f }, { u, v }, normal);
            }
        }

        for (uint32_t lat = 0; lat < kLatitude; ++lat)
        {
            for (uint32_t lon = 0; lon < kLongitude; ++lon)
            {
                uint32_t curr = lat * (kLongitude + 1) + lon;
                uint32_t next = (lat + 1) * (kLongitude + 1) + lon;

                model->AddIndex(curr);
                model->AddIndex(next);
                model->AddIndex(curr + 1);

                model->AddIndex(curr + 1);
                model->AddIndex(next);
                model->AddIndex(next + 1);
            }
        }

        model->UpdateVertexBuffer();
        model->UpdateIndexBuffer();
    }

    void MeshBuilder::BuildPetal(Model* model)
    {
        model->ClearVertexData();

        // パラメータ
        const uint32_t kDiv = 32; // 分割数
        const float baseRadius = 0.18f; // 根元の半径
        const float tipHeight = 0.7f;   // 花びらの高さ
        const float angleSpan = std::numbers::pi_v<float> *0.85f; // 花びらの開き角度
        const float bulge = 0.18f;      // 側面のふくらみ
        const float tipSplit = 0.08f;   // 先端の割れ
        const float tipBulge = 0.10f;   // 先端のふくらみ

        // 頂点リスト
        std::vector<uint32_t> edgeIndices;

        // 根元の中心
        uint32_t centerIdx = model->GetVertexCount();
        model->AddVertex({ 0.0f, 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f }, { 0,0,1 });

        // 花びらの輪郭
        for (uint32_t i = 0; i <= kDiv; ++i)
        {
            float t = float(i) / float(kDiv);
            float angle = (t - 0.5f) * angleSpan;

            // 根元から先端までの補間
            float y = std::sin(angle) * baseRadius;
            float x = std::cos(angle) * baseRadius * 0.7f;
            y;
            x;

            // 花びらの先端に向かう補間
            float v = t;
            float bulgeFactor = static_cast<float>(std::pow(std::sin(std::numbers::pi_v<float> *t), 2));
            float px = std::sin(angle) * (baseRadius + bulge * bulgeFactor);
            float pz = std::cos(angle) * (baseRadius * 0.7f + bulge * bulgeFactor);
            float py = v * tipHeight;

            // 先端の割れ
            if (t > 0.45f && t < 0.55f)
            {
                float split = tipSplit * std::sin((t - 0.5f) * std::numbers::pi_v<float> *2.0f);
                py += tipBulge * std::sin(std::numbers::pi_v<float> *t);
                px += split;
            }

            // 先端に近いほど高さを上げる
            float tipRate = std::pow(v, 1.5f);
            float finalY = py;
            float finalX = px * (1.0f - tipRate) + 0.0f * tipRate;
            float finalZ = pz * (1.0f - tipRate) + 0.0f * tipRate;

            float u = t;
            edgeIndices.push_back(model->GetVertexCount());
            model->AddVertex({ finalX, finalY, finalZ, 1.0f }, { u, 1.0f - v }, { 0,0,1 });
        }

        // 根元の扇形
        for (uint32_t i = 0; i < kDiv; ++i)
        {
            model->AddIndex(centerIdx);
            model->AddIndex(edgeIndices[i]);
            model->AddIndex(edgeIndices[i + 1]);
        }

        // 先端の割れを強調するため、先端に追加の三角形を作ることもできます（省略可）

        model->UpdateVertexBuffer();
        model->UpdateIndexBuffer();
    }

    void MeshBuilder::BuildTriangle(Model* model)
    {
        model->ClearVertexData();

        // XY平面上の正三角形
        float scale = 1.0f; // 拡大率
        Vector3 v0 = { 0.0f, 0.5f * scale, 0.0f };
        Vector3 v1 = { -0.433f * scale, -0.25f * scale, 0.0f };
        Vector3 v2 = { 0.433f * scale, -0.25f * scale, 0.0f };


        uint32_t base = model->GetVertexCount();
        model->AddVertex({ v0.x, v0.y, v0.z, 1.0f }, { 0.5f, 0.0f }, { 0,0,1 });
        model->AddVertex({ v1.x, v1.y, v1.z, 1.0f }, { 0.0f, 1.0f }, { 0,0,1 });
        model->AddVertex({ v2.x, v2.y, v2.z, 1.0f }, { 1.0f, 1.0f }, { 0,0,1 });

        model->AddIndex(base + 0);
        model->AddIndex(base + 1);
        model->AddIndex(base + 2);

        model->UpdateVertexBuffer();
        model->UpdateIndexBuffer();
    }

    void MeshBuilder::BuildCube(Model* model)
    {
        model->ClearVertexData();

        // 1辺0.2fの立方体（中心原点）
        const float h = 1.0f;

        // 頂点座標（x, y, z, w）
        const Vector4 positions[8] =
        {
            { -h, -h, -h, 1.0f }, // 0: 左下手前
            {  h, -h, -h, 1.0f }, // 1: 右下手前
            {  h,  h, -h, 1.0f }, // 2: 右上手前
            { -h,  h, -h, 1.0f }, // 3: 左上手前
            { -h, -h,  h, 1.0f }, // 4: 左下奥
            {  h, -h,  h, 1.0f }, // 5: 右下奥
            {  h,  h,  h, 1.0f }, // 6: 右上奥
            { -h,  h,  h, 1.0f }, // 7: 左上奥
        };

        // UV座標（各面共通）
        const Vector2 uvs[4] =
        {
            {0.0f, 1.0f}, // 左下
            {1.0f, 1.0f}, // 右下
            {1.0f, 0.0f}, // 右上
            {0.0f, 0.0f}, // 左上
        };

        // 各面の頂点インデックスと法線
        struct Face
        {
            int idx[4];
            Vector3 normal;
        };
        const Face faces[6] =
        {
            // 前面
            { {0, 1, 2, 3}, { 0,  0, -1} },
            // 背面
            { {5, 4, 7, 6}, { 0,  0,  1} },
            // 左面
            { {4, 0, 3, 7}, {-1,  0,  0} },
            // 右面
            { {1, 5, 6, 2}, { 1,  0,  0} },
            // 上面
            { {3, 2, 6, 7}, { 0,  1,  0} },
            // 下面
            { {4, 5, 1, 0}, { 0, -1,  0} },
        };

        // 頂点追加
        for (int f = 0; f < 6; ++f)
        {
            uint32_t base = model->GetVertexCount();
            for (int v = 0; v < 4; ++v)
            {
                model->AddVertex(positions[faces[f].idx[v]], uvs[v], faces[f].normal);
            }
            // インデックス（2三角形）
            model->AddIndex(base + 0);
            model->AddIndex(base + 1);
            model->AddIndex(base + 2);
            model->AddIndex(base + 2);
            model->AddIndex(base + 3);
            model->AddIndex(base + 0);
        }

        model->UpdateVertexBuffer();
        model->UpdateIndexBuffer();

    }

    void MeshBuilder::BuildLine(Model* model)
    {
        model->ClearVertexData();

        // 原点からY軸方向に1.0fの線分
        Vector3 start = { 0.0f, 0.0f, 0.0f };
        Vector3 end = { 0.2f, 1.2f, 0.2f };
        uint32_t base = model->GetVertexCount();

        model->AddVertex({ start.x, start.y, start.z, 1.0f }, { 0.0f, 0.0f }, { 0,0,1 });
        model->AddVertex({ end.x,   end.y,   end.z,   1.0f }, { 1.0f, 1.0f }, { 0,0,1 });
        model->AddIndex(base + 0);
        model->AddIndex(base + 1);
        model->UpdateVertexBuffer();
        model->UpdateIndexBuffer();
    }
}