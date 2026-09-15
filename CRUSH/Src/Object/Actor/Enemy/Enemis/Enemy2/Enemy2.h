#pragma once
#include "../EnemyBase.h"

class Enemy2 : public EnemyBase
{
public:
	// コピーコンストラクタ(複製用)
	Enemy2(const Enemy2&) = default;
	Enemy2();

	// 複製
	EnemyBase* Clone() const override { return new Enemy2(*this); }
	
	void Load(void) override;
	void Init(void) override;
	void Reset(void) override;
	void Update(void) override;
	void Draw(void) override;

private:

};
