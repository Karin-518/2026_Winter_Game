#pragma once
#include "../../../Common/State/StateBase.h"
#include "PlayerState.h"

class Player;

class PlayerKnockbackState : public StateBase<Player, PLAYER_STATE>
{
	void Enter(Player& owner) override;

	PLAYER_STATE Update(Player& owner) override;

	void Exit(Player& owner) override;

private:

	// 吹き飛びの段階
	enum class KNOCKBACK_PHASE
	{
		FLY,		// 吹き飛んでいる
		STANDING,	// 起き上がっている
	};

	// 今の段階
	KNOCKBACK_PHASE phase_ = KNOCKBACK_PHASE::FLY;

	// 吹き飛び中
	void UpdateFly(Player& owner);

	// 起き上がり中
	PLAYER_STATE UpdateStanding(Player& owner);
};