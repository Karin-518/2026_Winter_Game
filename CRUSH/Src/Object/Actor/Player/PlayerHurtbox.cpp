#include "../../Collider/Capsule/ColliderCapsule.h"

#include "PlayerHurtbox.h"

PlayerHurtbox::PlayerHurtbox(ActorBase* owner, const ColliderCapsule* bodyCapsule)
	:
	Hurtbox(ColliderBase::TAG::PLAYER, owner, bodyCapsule->GetRadius()),
	bodyCapsule_(bodyCapsule)
{
}

PlayerHurtbox::~PlayerHurtbox(void)
{
}

void PlayerHurtbox::Update(void)
{
	SetSegment(bodyCapsule_->GetPosDown(), bodyCapsule_->GetPosTop());
}