#include "../../../Common/AnimationController/AnimationController.h"
#include "../Player.h"

#include "PlayerDeadState.h"

void PlayerDeadState::Enter(Player& owner)
{
	// 死亡アニメーションを再生
	owner.GetAnimationController()->Play(static_cast<int>(Player::ANIM_TYPE::DEAD),false);
}

PLAYER_STATE PlayerDeadState::Update(Player& owner)
{
	// 死亡したら、ほかのステートに移らない
    return PLAYER_STATE::NONE;
}