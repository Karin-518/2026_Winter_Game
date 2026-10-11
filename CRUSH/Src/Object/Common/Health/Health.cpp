#include "Health.h"

Health::Health(void)
	:
	hp_(0),
	maxHp_(0),
	invincibleFrame_(0),
	isHalfNotified_(false)
{
}

void Health::Init(int maxHp)
{
	maxHp_ = maxHp;
	hp_ = maxHp_;
	invincibleFrame_ = 0;
	isHalfNotified_ = false;
}

void Health::Update(void)
{
	if (invincibleFrame_ > 0)
	{
		invincibleFrame_--;
	}
}

bool Health::Damage(int damage)
{
	// 倒れている・無敵ならダメージを受けない
	if (hp_ <= 0 || IsInvincible()) return false;

	// HPを減らす
	hp_ -= damage;

	if (hp_ <= 0)
	{
		hp_ = 0;
		Notify(onDead_);
		return true;
	}

	// 半分以下になったら、1回だけ通知
	if (!isHalfNotified_ && hp_ * 2 <= maxHp_)
	{
		isHalfNotified_ = true;
		Notify(onHalf_);
	}

	return true;
}

void Health::SetInvincible(int frame)
{
	// 今残っている無敵時間のほうが長ければ上書きしない
	if (frame <= invincibleFrame_) return;

	invincibleFrame_ = frame;
}

void Health::Notify(const std::function<void(void)>& func) const
{
	if (func) { func(); }
}
