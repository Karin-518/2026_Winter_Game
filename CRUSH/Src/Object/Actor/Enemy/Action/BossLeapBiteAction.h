#pragma once
#include "BossAttackAction.h"

class BossLeapBiteAction : public BossAttackAction
{
public:

	// 攻撃本体の間、舞フレーム呼ばれる
	bool UpdateActive(Boss& owner) override;

	SOUND_ID GetSignSoundID(void) const override;
	SOUND_ID GetAttackSoundID(void) const override;
};