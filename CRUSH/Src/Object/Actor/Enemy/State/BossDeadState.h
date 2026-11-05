#pragma once
#include "../../../Common/State/StateBase.h"
#include "BossState.h"

class Boss;

class BossDeadState : public StateBase<Boss, BOSS_STATE>
{
	void Enter(Boss& owner) override;

	BOSS_STATE Update(Boss& owner) override;
};