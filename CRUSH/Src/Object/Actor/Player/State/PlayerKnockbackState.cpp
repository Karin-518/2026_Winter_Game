#include "../../../Common/AnimationController/AnimationController.h"
#include "../Player.h"

#include "PlayerKnockbackState.h"

namespace
{
	// 1フレームごとに速さにかける割合
	constexpr float KNOCKBACK_DECAY_RATE = 0.9f;

	// 吹き飛びが終わった後も続ける無敵のフレーム数
	constexpr int KNOCKBACK_INVINCIBLE_FRAME = 20;

	// 吹き飛びアニメをどこまで見せたら起き上がるか
	constexpr float FLY_END_RATE = 0.9f;

	// 起き上がりアニメをどこまで見せたら待機へ移るか
	constexpr float STANDING_END_RATE = 0.7f;
}

void PlayerKnockbackState::Enter(Player& owner)
{
	phase_ = KNOCKBACK_PHASE::FLY;
	owner.StartKnockback();
}

PLAYER_STATE PlayerKnockbackState::Update(Player& owner)
{
	// 吹き飛び中は無敵を切らさない
	owner.ExtendInvincible(KNOCKBACK_INVINCIBLE_FRAME);

	if (phase_ == KNOCKBACK_PHASE::FLY)
	{
		UpdateFly(owner);
		return PLAYER_STATE::NONE;
	}

	return UpdateStanding(owner);
}

void PlayerKnockbackState::UpdateFly(Player& owner)
{
	// 後ろへ滑って、少しずつ遅くする
	owner.UpdateKnockbackMove(KNOCKBACK_DECAY_RATE);

	// 倒れきっていなければ続ける
	if (owner.GetAnimationController()->GetRate() < FLY_END_RATE) return;

	// 起き上がりへ
	owner.StartStanding();
	phase_ = KNOCKBACK_PHASE::STANDING;
}

PLAYER_STATE PlayerKnockbackState::UpdateStanding(Player& owner)
{
	// 立ち上がり切っていなければ続ける
	if (owner.GetAnimationController()->GetRate() < STANDING_END_RATE)
	{
		return PLAYER_STATE::NONE;
	}

	// 起き上がったら待機へ
	return PLAYER_STATE::IDLE;
}
