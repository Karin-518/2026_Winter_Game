#pragma once
#include "BossAttackAction.h"

class BossTailSpinAction : public BossAttackAction
{
public:

	// コンストラクタ
	BossTailSpinAction(void);

	// デストラクタ
	~BossTailSpinAction(void) override;

	// 溜め中
	void UpdateWindup(Boss& owner) override;

	// 攻撃本体に入ったとき
	void OnActiveStart(Boss& owner) override;

	// 攻撃本体の間
	bool UpdateActive(Boss& owner) override;

	SoundID GetSignSoundID(void) const override;
	SoundID GetAttackSoundID(void) const override;

private:

	float spunRad_;
};