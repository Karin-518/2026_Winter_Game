#pragma once
#include "../EnemyBase.h"

class Enemy1 : public EnemyBase
{
public:
	Enemy1() = default;

	// コピーコンストラクタ(複製用)
	Enemy1(const Enemy1& rhs);

	// 複製
	EnemyBase* Clone() const override { return new Enemy1(*this); }

	void Load(void) override;
	void Init(void) override;
	void Reset(void) override;
	void Update(void) override;
	void Draw(void) override;

private:

};
