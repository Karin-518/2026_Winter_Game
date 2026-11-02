#pragma once
#include <vector>
#include "ColliderBase.h"

class ActorBase;
class AttackController;
class Hurtbox;

class CollisionManager
{
public:

	// コンストラクタ
	CollisionManager(void);

	// デストラクタ
	~CollisionManager(void);

	// 更新
	void Update(void);

	// 攻撃を登録
	void AddAttack(const AttackController* attack, ActorBase* owner, ColliderBase::TAG tag);

	// 食らい判定を登録
	void AddHurtbox(const Hurtbox* hurtbox);

	// 登録をすべて外す
	void Clear(void);

private:

	// 登録した攻撃
	struct AttackEntry
	{
		const AttackController* attack_;	// 攻撃の判定
		ActorBase* owner_;					// 攻撃した持ち主
		ColliderBase::TAG tag_;				// 攻撃した側の衝突種別
	};

	// 登録した攻撃
	std::vector<AttackEntry> attacks_;

	// 登録した食らい判定
	std::vector<const Hurtbox*> hurtboxes_;

	// 1つの攻撃をすべての食らい判定と調べる
	void CheckAttack(const AttackEntry& entry);

	// 判定をする相手か、攻撃した側と違う衝突種別か
	bool IsTarget(const AttackEntry& entry, const Hurtbox& hurtbox) const;

	// 攻撃の形と食らい判定が重なっているか
	bool IsOverlap(const AttackEntry& entry, const Hurtbox& hurtbox) const;
};