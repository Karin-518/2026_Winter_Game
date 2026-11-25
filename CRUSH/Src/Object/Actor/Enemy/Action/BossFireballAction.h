#pragma once
#include "BossAttackAction.h"

class BossFireballAction : public BossAttackAction
{
public:

	// コンストラクタ
	BossFireballAction(void);

	// デストラクタ
	~BossFireballAction(void) override;

	// 攻撃本体に入ったとき
	void OnActiveStart(Boss& owner) override;

	// 攻撃本体の間
	bool UpdateActive(Boss& owner) override;

	SOUND_ID GetSignSoundID(void) const override;
	SOUND_ID GetAttackSoundID(void) const override;
};