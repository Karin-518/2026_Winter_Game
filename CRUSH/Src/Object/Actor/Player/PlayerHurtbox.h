#pragma once
#include "../../Collider/Hurtbox.h"

class ColliderCapsule;

class PlayerHurtbox : public Hurtbox
{
public:

	// コンストラクタ
	PlayerHurtbox(ActorBase* owner, const ColliderCapsule* bodyCapsule);

	// �?��トラクタ
	~PlayerHurtbox(void) override;

	// 更新
	void Update(void) override;

private:

	// 位置を合わせ�?
	const ColliderCapsule* bodyCapsule_;
};