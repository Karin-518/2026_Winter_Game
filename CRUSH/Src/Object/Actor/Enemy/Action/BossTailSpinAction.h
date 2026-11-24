#pragma once
#include "BossAttackAction.h"

class BossTailSpinAction : public BossAttackAction
{
public:

	// コンストラクタ
	BossTailSpinAction(void);

	// デストラクタ
	~BossTailSpinAction(void) override;

	// 攻撃本体の間
	bool UpdateActive(Boss& owner) override;

	SOUND_ID GetSignSoundID(void) const override;
	SOUND_ID GetAttackSoundID(void) const override;
};