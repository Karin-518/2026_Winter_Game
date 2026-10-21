#include "../Player.h"

#include "PlayerDodgeState.h"

namespace
{
    // 回避全体のフレーム
    constexpr int DODGE_FRAME = 60;

    // 無敵のフレーム
    constexpr int INVINCIBLE_FRAME = 15;

    // 回避の速さ
    constexpr float DODGE_SPEED = 10.0f;
}

void PlayerDodgeState::Enter(Player& owner)
{
    frame_ = 0;

    owner.StartDodge(INVINCIBLE_FRAME);
}

PLAYER_STATE PlayerDodgeState::Update(Player& owner)
{
    // 経過フレームを進める
    frame_++;

    // フレームが経過したら待機へ
    if (frame_ >= DODGE_FRAME)
    {
        return PLAYER_STATE::IDLE;
    }

    owner.MoveForward(DODGE_SPEED);

    return PLAYER_STATE::NONE;
}
