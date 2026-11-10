#include "HitStop.h"

HitStop::HitStop(void)
    :
    remainFrame_(0)
{
}

HitStop::~HitStop(void)
{
}

void HitStop::Start(int frame)
{
    // 新しいフレーム数が今の残りのフレーム数以下なら、早期リターン
    if (remainFrame_ >= frame) return;

    remainFrame_ = frame;
}

void HitStop::Update(void)
{
    // ストップ中ではないなら、早期リターン
    if (!IsStopping()) return;

    remainFrame_--;
}

bool HitStop::IsStopping(void) const
{
    // 0以下なら止まっていない
    if (remainFrame_ <= 0) return false;

    return true;
}