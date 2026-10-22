#pragma once
#include "../ActorBase.h"
#include "../../Common/State/StateContext.h"
#include "../../Common/Health/Health.h"
#include "../../Common/Attack/AttackController.h"
#include "../../Common/Attack/AttackParam.h"
#include "State/BossState.h"

class Player;

class Boss : public ActorBase
{
public:

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		WALK,
		ATTACK_BITE,	// 噛みつき

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
	bool IsDead(void) const { return health_.IsDead(); }

	// 攻撃開始
	void StartAttack(void);

	// 攻撃終了
	void EndAttack(void);

	// 自分の攻撃が当たったときに呼ばれる
	void OnAttackHit(void);

	// 攻撃の判定
	const AttackController& GetAttackController(void) const { return attackController_; }

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
	AttackParam attackParam_;

private:

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

	// HPが0になったときに呼ぶ
	void OnDead(void);

	// 攻撃データの初期化
	void InitAttackParam(void);

	// プレイヤーの方向を見る
	void FacePlayer(void);
};