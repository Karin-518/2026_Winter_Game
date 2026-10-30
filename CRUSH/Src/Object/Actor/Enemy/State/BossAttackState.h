#pragma once
#include "../../../Common/State/StateBase.h"
#include "BossState.h"

class Boss;

class BossAttackState : public StateBase<Boss, BOSS_STATE>
{
public:

	void Enter(Boss& owner) override;

	BOSS_STATE Update(Boss& owner) override;

	void Exit(Boss& owner) override;
};