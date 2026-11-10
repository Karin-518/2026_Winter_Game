#pragma once
#include <memory>
#include "../../Common/State/StateContext.h"
#include "../../Common/Health/Health.h"
#include "../../Common/Attack/AttackController.h"
#include "../../Common/Attack/AttackParam.h"
#include "../ActorBase.h"
#include "State/PlayerState.h"
#include "PlayerAttackType.h"
#include "PlayerWeapon.h"

class Camera;
class PlayerHurtbox;
class Hurtbox;

class Player : public ActorBase
{
public:

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		WALK,
		RUN,
		ATTACK_1,
		ROLL,
		HIT,
		DEAD,
		KNOCKBACK,
		STANDING,

		MAX,
	};

	// コンストラクタ
	Player(Camera* camera);

	// デストラクタ
	~Player(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// アニメーションの取得
	AnimationController* GetAnimationController(void) const { return animationController_; }

	// 移動用の入力処理
	void MoveByInput(float speed);

	// ヘルスの取得
	const Health& GetHealth(void) const { return health_; }

	// 攻撃が当たりそうなときに呼ばれる
	bool OnHit(const HitInfo& hitInfo) override;

	// 倒れたか
	bool IsDead(void) const;

	// 攻撃開始
	void StartAttack(PLAYER_ATTACK_ID id);

	// 攻撃終了
	void EndAttack(void);

	// 自分の攻撃が当たったときに呼ばれる
	void OnAttackHit(void) override;

	// 攻撃の判定
	const AttackController& GetAttackController(void) const { return attackController_; }

	// 回避開始
	void StartDodge(int invincibleFrame);

	// 食らい判定
	const Hurtbox* GetHurtbox(void) const;

	// のけぞり開始
	void StartHit(void);

	// 無敵の残り香指定フレームより短ければ延ばす
	void ExtendInvincible(int frame);

	// 吹き飛び開始
	void StartKnockback(void);

	// 吹き飛ぶ向きへ進んで少しずつ遅くする
	void UpdateKnockbackMove(float decayRate);

	// 吹き飛びの速さ
	float GetKnockbackSpeed(void) const { return knockbackSpeed_; }

	// 起き上がり開始
	void StartStanding(void);

	// 吹き飛ばされて倒れているか
	bool IsPlayerKnockedDown(void) const { return state_.GetStateId() == PLAYER_STATE::KNOCKBACK; }

	// 被弾のビネットを通常に戻す
	void EndDamageVignette(void);

protected:

	// リソースロード
	void InitLoad(void) override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// 当たり判定の作成
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void) override;

private:

	// カメラ
	Camera* camera_;

	// ヘルス
	Health health_;

	// 攻撃の判定
	AttackController attackController_;

	// ステートを管理
	StateContext<Player, PLAYER_STATE> state_;

	// 攻撃ごとのデータ
	AttackParam attackParams_[static_cast<int>(PLAYER_ATTACK_ID::MAX)];

	// 剣
	PlayerWeapon weapon_;

	// 食らい判定
	std::unique_ptr <PlayerHurtbox> hurtbox_;

	// 吹き飛ぶ向き
	VECTOR knockbackDir_;

	// 吹き飛びの速さ
	float knockbackSpeed_;

	// HPが0になったときに呼ぶ
	void OnDead(void);

	// 攻撃データの初期化
	void InitAttackParam(void);

	// 移動入力を合成した方向
	VECTOR CalcInputDir(void) const;

	// 移動入力を合成した方向
	VECTOR ToWorldDir(const VECTOR& localDir) const;

	// 入力があれば、その方向へすぐ向く
	void FaceInputDir(void);

	// 当たった情報から吹き飛ぶ向きと速さを決める
	void PrepareKnockback(const HitInfo& hitInfo);

	// 吹き飛ばす横向き
	VECTOR CalcKnockbackSideDir(const HitInfo& hitInfo) const;
};