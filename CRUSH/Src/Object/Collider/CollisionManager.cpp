#include "../Actor/ActorBase.h"
#include "../Common/Attack/AttackController.h"
#include "../Common/Attack/AttackParam.h"
#include "Hurtbox.h"

#include "CollisionManager.h"

CollisionManager::CollisionManager(void)
{
}

CollisionManager::~CollisionManager(void)
{
}

void CollisionManager::Update(void)
{
	for (const auto& entry : attacks_)
	{
		CheckAttack(entry);
	}
}

void CollisionManager::AddAttack(const AttackController* attack, ActorBase* owner, ColliderBase::TAG tag)
{
	attacks_.push_back({ attack, owner, tag });
}

void CollisionManager::AddHurtbox(const Hurtbox* hurtbox)
{
	// nullptr は登録しない
	if (hurtbox == nullptr) return;

	hurtboxes_.push_back(hurtbox);
}

void CollisionManager::Clear(void)
{
	attacks_.clear();
	hurtboxes_.clear();
}

void CollisionManager::CheckAttack(const AttackEntry& entry)
{
	// 判定区間の外、またはこの1振りでもう当たった
	if (!entry.attack_->CanHit()) return;

	for (const Hurtbox* hurtbox : hurtboxes_)
	{
		// 違う陣営で、攻撃の形と重なっている相手だけ見る
		if (!IsTarget(entry, *hurtbox)) continue;
		if (!IsOverlap(entry, *hurtbox)) continue;

		// 当たった側に知らせる
		bool isDamaged = hurtbox->GetOwner()->OnHit(entry.attack_->GetParam()->damage);
		if (!isDamaged) continue;

		// 攻撃した側に知らせる
		entry.owner_->OnAttackHit();

		// 1振りで当たるのは1回
		return;
	}
}

bool CollisionManager::IsTarget(const AttackEntry& entry, const Hurtbox& hurtbox) const
{
	return entry.tag_ != hurtbox.GetTag();
}

bool CollisionManager::IsOverlap(const AttackEntry& entry, const Hurtbox& hurtbox) const
{
	const AttackParam* param = entry.attack_->GetParam();
	const VECTOR& ownerPos = entry.owner_->GetPos();
	VECTOR forward = entry.owner_->GetForward();

	// 攻撃の形のおおよその真ん中
	float distanceToCenter = param->offset + param->length * 0.5f;
	VECTOR attackCenter = VAdd(ownerPos, VScale(forward, distanceToCenter));

	// 食らい判定のうち、攻撃の真ん中に一番近い点を円として調べる
	VECTOR closestPos = hurtbox.GetClosestPos(attackCenter);

	return entry.attack_->IsHit(ownerPos, forward, closestPos, hurtbox.GetRadius());
}