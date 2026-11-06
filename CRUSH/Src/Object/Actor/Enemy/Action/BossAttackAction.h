#pragma once
#include "../../../../Audio/SoundInfo.h"

class Boss;

class BossAttackAction
{
public:

	// コンストラクタ
	BossAttackAction(void) = default;

	// デストラクタ
	virtual ~BossAttackAction(void) = default;

	// 溜めに入った時
	virtual void OnWindupStart(Boss& owner) {}

	// 溜め中
	virtual void UpdateWindup(Boss& owner) {}

	// 攻撃本体に入った時
	virtual void OnActiveStart(Boss& owner) {}

	/// <summary>
	/// 攻撃本体の間
	/// </summary>
	/// <param name="owner">攻撃しているボス</param>
	/// <returns>trueなら攻撃本体を終えて、隙へ進む</returns>
	virtual bool UpdateActive(Boss& owner) = 0;

	virtual SoundID GetSignSoundID(void) const = 0;
	virtual SoundID GetAttackSoundID(void) const = 0;
};