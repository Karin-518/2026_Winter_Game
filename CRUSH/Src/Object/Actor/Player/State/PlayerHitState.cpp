#include "../../../Common/AnimationController/AnimationController.h"
#include "../Player.h"

#include "PlayerHitState.h"

namespace
{
	// のけぞりが終わった後も続ける無敵のフレーム数
	constexpr int AFTER_HIT_INVINCIBLE_FRAME = 20;
}

void PlayerHitState::Enter(Player& owner)
{
    owner.StartHit();
}

PLAYER_STATE PlayerHitState::Update(Player& owner)
{
	// のけぞり中は無敵を切らさない
	owner.ExtendInvincible(AFTER_HIT_INVINCIBLE_FRAME);

	// のけぞりが終わったら待機へ
	if (owner.GetAnimationController()->IsEnd())
	{
		return PLAYER_STATE::IDLE;
	}

	// のけぞり中は入力を受け付けない
	return PLAYER_STATE::NONE;
}
