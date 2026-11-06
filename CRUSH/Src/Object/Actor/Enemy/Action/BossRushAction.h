#pragma once
#include "BossAttackAction.h"

class BossRushAction : public BossAttackAction
{
public:

	// コンストラクタ
	BossRushAction(void);

	// デストラクタ
	~BossRushAction(void) override;

	// 溜め中
	void UpdateWindup(Boss& owner) override;

	// 攻撃本体に入ったとき
	void OnActiveStart(Boss& owner) override;

	// 攻撃本体の間
	bool UpdateActive(Boss& owner) override;

	SoundID GetSignSoundID(void) const override;
	SoundID GetAttackSoundID(void) const override;

private:

	// 走る予定の距離
	float chargeDistance_;

	// 走った距離
	float movedDistance_;

	// 走り始めてからのフレーム数
	int chargeFrame_;
};