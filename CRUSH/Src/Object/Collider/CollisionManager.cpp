#include "../Actor/ActorBase.h"
#include "../Actor/Enemy/BossFireball.h"
#include "../Common/Attack/AttackController.h"
#include "../Common/Attack/AttackParam.h"
#include "../Common/Attack/HitInfo.h"
#include "Hurtbox.h"

#include "CollisionManager.h"

CollisionManager::CollisionManager(void)
	:
	attacks_(),
	hurtboxes_(),
	fireballs_(nullptr)
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

	// ボスの火の玉
	CheckFireballs();
}

void CollisionManager::AddAttack(const AttackController* attack, ActorBase* owner, ColliderBase::TAG tag)
{
	attacks_.push_back({ attack, owner, tag });
}

void CollisionManager::AddFireballs(std::vector<std::unique_ptr<BossFireball>>* fireballs)
{
	fireballs_ = fireballs;
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
	fireballs_ = nullptr;
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

		// 当たった側へ渡す情報
		HitInfo hitInfo = MakeHitInfo(entry, *hurtbox);

		// 当たった側に知らせる
		bool isDamaged = hurtbox->GetOwner()->OnHit(hitInfo);
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
	// カプセル同士の判定はAttackControllerに任せる
	return entry.attack_->IsHit(hurtbox);
}

HitInfo CollisionManager::MakeHitInfo(const AttackEntry& entry, const Hurtbox& hurtbox) const
{
	// 攻撃のデータ
	const AttackParam* param = entry.attack_->GetParam();

	// 攻撃した側の位置
	const VECTOR& attackerPos = entry.owner_->GetPos();

	HitInfo hitInfo;
	hitInfo.damage = param->damage;
	hitInfo.attackerPos = entry.owner_->GetPos();
	hitInfo.reaction = param->reaction;
	hitInfo.knockbackSpeed = param->knockbackSpeed;
	hitInfo.attackerForward = entry.owner_->GetForward();
	hitInfo.knockbackSideRate = param->knockbackSideRate;
	hitInfo.hitPos = CalcHitSurfacePos(hurtbox, attackerPos);

	return hitInfo;
}

VECTOR CollisionManager::CalcHitSurfacePos(const Hurtbox& hurtbox, const VECTOR& attackerPos) const
{
	// 食らい判定の芯の上で、攻撃した側に一番近い点
	VECTOR closestPos = hurtbox.GetClosestPos(attackerPos);

	// 芯から攻撃した側への向き（上下は見ない）
	VECTOR toAttacker = VSub(attackerPos, closestPos);
	toAttacker.y = 0.0f;

	// 向きが分からないときは、芯の上の点をそのまま使う
	if (VSize(toAttacker) <= 0.0f) return closestPos;

	// 半径の分だけ外へ出して、カプセルの表面にする
	VECTOR surfaceOffset = VScale(VNorm(toAttacker), hurtbox.GetRadius());

	return VAdd(closestPos, surfaceOffset);
}

void CollisionManager::CheckFireballs(void)
{
	// 登録されていなければ何もしない
	if (fireballs_ == nullptr) return;

	for (auto& fireball : *fireballs_)
	{
		// もう消えている火の玉は調べない
		if (!fireball->IsAlive()) continue;

		CheckFireball(*fireball);
	}
}

void CollisionManager::CheckFireball(BossFireball& fireball)
{
	for (const Hurtbox* hurtbox : hurtboxes_)
	{
		// ボスの火の玉なので、ボス自身には当てない
		if (hurtbox->GetTag() == ColliderBase::TAG::ENEMY) continue;
		if (!IsFireballOverlap(fireball, *hurtbox)) continue;

		// 当たった側へ渡す情報
		HitInfo hitInfo = MakeFireballHitInfo(fireball, *hurtbox);

		// 当たった側に知らせる（無敵中ならダメージを受けない）
		bool isDamaged = hurtbox->GetOwner()->OnHit(hitInfo);

		// ダメージを受けなかったら、火の玉はすり抜けて飛び続ける
		if (!isDamaged) continue;

		// 当たった火の玉は消える
		fireball.OnHit();
		return;
	}
}

bool CollisionManager::IsFireballOverlap(const BossFireball& fireball, const Hurtbox& hurtbox) const
{
	// 火の玉の球と、食らい判定のカプセルが重なっているか
	int hitResult = HitCheck_Sphere_Capsule(
		fireball.GetPos(), fireball.GetRadius(),
		hurtbox.GetStartPos(), hurtbox.GetEndPos(), hurtbox.GetRadius());

	return hitResult == TRUE;
}

HitInfo CollisionManager::MakeFireballHitInfo(const BossFireball& fireball, const Hurtbox& hurtbox) const
{	
	// 火の玉の位置（のけぞる向きを、火の玉が飛んできた側にするため）
	const VECTOR& fireballPos = fireball.GetPos();

	HitInfo hitInfo;
	hitInfo.damage = fireball.GetDamage();
	hitInfo.attackerPos = fireballPos;
	hitInfo.attackerForward = fireball.GetMoveDir();
	hitInfo.hitPos = CalcHitSurfacePos(hurtbox, fireballPos);

	return hitInfo;
}
