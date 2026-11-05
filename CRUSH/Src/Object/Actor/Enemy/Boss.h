#pragma once
#include <vector>
#include <memory>
#include "../ActorBase.h"
#include "../../Common/State/StateContext.h"
#include "../../Common/Health/Health.h"
#include "../../Common/Attack/AttackController.h"
#include "../../Common/Attack/AttackParam.h"
#include "BossAttackSelector.h"
#include "State/BossState.h"
#include "ColliderPart.h"

class Player;
class BossPart;

class Boss : public ActorBase
{
public:

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		WALK,
		RUN,
		ATTACK_BITE,	// 噛みつき
		DEAD,

		MAX,
	};

	// コンストラクタ
	Boss(Player* player);

	// デストラクタ
	~Boss(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// アニメーションの取得
	AnimationController* GetAnimationController(void) const { return animationController_; }

	// ヘルスの取得
	const Health& GetHealth(void) { return health_; }

	// 攻撃が当たりそうなときに呼ばれる
	void OnHit(int damage) override;

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

	// 攻撃を選ぶ
	bool SelectAttack(void);

private:

	// プレイヤー
	Player* player_;

	// ヘルス
	Health health_;

	// ステートを管理
	StateContext<Boss, BOSS_STATE> state_;

	// 攻撃の判定
	AttackController attackController_;

	// 攻撃ごとのデータ
	AttackParam attackParams_[static_cast<int>(BOSS_ATTACK_ID::MAX)];

	// 攻撃を決めるセレクター
	BossAttackSelector attackSelector_;

	// 次に出す攻撃
	BOSS_ATTACK_ID nextAttackId_;

	// 部位の当たり判定
	std::vector<std::unique_ptr<ColliderPart>> parts_;

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

	// 部位の初期化
	void InitParts(void);

	// HPが0になったときに呼ぶ
	void OnDead(void);

	// 攻撃データの初期化
	void InitAttackParam(void);

	// プレイヤーの方向を見る
	void FacePlayer(void);

	// プレイヤーとボスとの距離を測る
	float CalcDistanceToPlayer(void) const;
};