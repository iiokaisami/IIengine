#pragma once

#include "../../../baseObject/BaseBullet.h"

class ShockWave : public BaseBullet
{


};

// 判定はTriggerのみ使用で当たった瞬間だけダメージ
// 被弾or回避成功で徐々に消える形に