#pragma once
#include "../../Collider/Hurtbox.h"

class ColliderCapsule;

class PlayerHurtbox : public Hurtbox
{
public:

	// コンストラクタ
	PlayerHurtbox(ActorBase* owner, const ColliderCapsule* bodyCapsule);

	// デストラクタ
	~PlayerHurtbox(void) override;

	// 更新
	void Update(void) override;

private:

	// 体のカプセル
	const ColliderCapsule* bodyCapsule_;
};