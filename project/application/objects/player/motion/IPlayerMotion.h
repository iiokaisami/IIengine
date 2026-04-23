#pragma once

class Player;

class IPlayerMotion 
{
public:
    virtual ~IPlayerMotion() = default;

    // 演出開始
    virtual void Start(Player& pPlayer) = 0;

    // 毎フレーム更新。完了したら true を返す
    virtual bool Update(Player& pPlayer) = 0;

    virtual bool IsActive() const = 0;
    virtual bool IsComplete() const = 0;

    // 再利用したい場合に備えて
    virtual void Reset() = 0;
};