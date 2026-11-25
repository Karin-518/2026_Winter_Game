#pragma once
#include <vector>
#include <memory>
#include "../../Common/State/StateContext.h"
#include "../../Common/Health/Health.h"
#include "../../Common/Attack/AttackController.h"
#include "../../Common/Attack/AttackParam.h"
#include "../../Common/Bone/BoneSegment.h"
#include "../../Collider/Pushbox.h"
#include "../ActorBase.h"
#include "State/BossState.h"
#include "Action/BossAttackAction.h"
#include "BossAttackSelector.h"
#include "ColliderPart.h"
#include "BossFireball.h"

class Player;
class HitStop;

class Boss : public ActorBase
{
public:

	// アニメーション種別
	enum class ANIM_TYPE
	{
		BITE_1,
		BITE_2,
		BITE_3,
		DEATH,
		FIRE_BALL,
		FLY_FIRE,
		IDLE,
		IMPACT_STRONG,
		IMPACT_WEAK,
		REPOSE,
		RUN,
		TAIL_1,
		TAIL_2,
		TAKEOFF,
		TORAND,
		TURNBACK,
		TURNLEFT,
		TURNRIGHT,
		WALK,

		MAX,
	};

	// コンストラクタ
	Boss(Player* player, HitStop* hitStop);

	// デストラクタ
	~Boss(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// アニメーションの取得
	AnimationController* GetAnimationController(void) const { return animationController_; }

	// ヘルスの取得
	const Health& GetHealth(void) const { return health_; }

	// 攻撃が当たりそうなときに呼ばれる
	bool OnHit(const HitInfo& hitInfo) override;

	// 倒れたか
	bool IsDead(void) const;

	// 攻撃開始
	void StartAttack(void);

	// 攻撃終了
	void EndAttack(void);

	// 自分の攻撃が当たったときに呼ばれる
	void OnAttackHit(void) override;

	// 攻撃の判定
	const AttackController& GetAttackController(void) const { return attackController_; }

	// 部位の食らい判定をまとめて取得
	std::vector<const Hurtbox*> GetHurtboxes(void) const;

	// 押し出し用のカプセルをまとめて取得
	std::vector<const Pushbox*> GetPushboxes(void) const;

	// 攻撃を選ぶ
	bool SelectAttack(void);

	// 攻撃判定のオン・オフ
	void SetAttackActive(bool isActive);

	// 今の攻撃データ
	const AttackParam& GetCurrentAttackParam(void) const { return attackParams_[static_cast<int>(nextAttackId_)]; }

	// 今の攻撃の動き
	BossAttackAction* GetCurrentAction(void) const { return attackActions_[static_cast<int>(nextAttackId_)].get(); }

	// プレイヤーの方向を見る
	void FacePlayer(void);

	// プレイヤーとボスとの距離を測る
	float CalcDistanceToPlayer(void) const;

	// 回転
	void RotateMoveDir(float addRad);

	// プレイヤーが倒れているか
	bool IsPlayerKnockedDown(void) const;

	// 火の玉を撃つ
	void ShootFireball(void);

	// 火の玉を取得
	std::vector<std::unique_ptr<BossFireball>>& GetFireballs(void) { return fireballs_; }

protected:

	// リソースロード
	void InitLoad(void) override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 当たり判定の作成
	void InitCollider(void) override;

	// 初期化後の個別処理
	void InitPost(void) override;

private:

	// プレイヤー
	Player* player_;

	// ヒットストップ
	HitStop* hitStop_;

	// ヘルス
	Health health_;

	// ステートを管理
	StateContext<Boss, BOSS_STATE> state_;

	// 攻撃の判定
	AttackController attackController_;

	// 攻撃ごとのデータ
	AttackParam attackParams_[static_cast<int>(BOSS_ATTACK_ID::MAX)];

	// 攻撃ごとの動き
	std::unique_ptr<BossAttackAction> attackActions_[static_cast<int>(BOSS_ATTACK_ID::MAX)];

	// 攻撃を決めるセレクター
	BossAttackSelector attackSelector_;

	// 次に出す攻撃
	BOSS_ATTACK_ID nextAttackId_;

	// 部位の当たり判定
	std::vector<std::unique_ptr<ColliderPart>> parts_;

	// 押し出し用のカプセル
	std::vector<std::unique_ptr<Pushbox>> pushboxes_;

	// 攻撃ごとの判定ボーン
	std::vector<BoneSegment> attackSegments_[static_cast<int>(BOSS_ATTACK_ID::MAX)];

	// 火の玉
	std::vector<std::unique_ptr<BossFireball>> fireballs_;

	// 火の玉を出す口のフレーム番号
	int mouthFrameIndex_;

	// 部位の初期化
	void InitParts(void);

	// 押し出し用のカプセルの初期化
	void InitPushboxes(void);

	// HPが0になったときに呼ぶ
	void OnDead(void);

	// 攻撃データの初期化
	void InitAttackParam(void);

	// 攻撃の判定のボーンを全部の攻撃分準備
	void InitAttackSegments(void);

	// 1つの攻撃の判定のボーンを準備
	void InitSegmentsOfAttack(int attackIndex);

	// 今の攻撃のカプセルをボーンの位置に合わせる
	void UpdateAttackCapsules(void);

	// 火の玉の更新
	void UpdateFireballs(void);
};