#pragma once
#include "../ActorBase.h"
#include "../../Common/State/StateContext.h"
#include "../../Common/Health/Health.h"
#include "../../Common/Attack/AttackController.h"
#include "../../Common/Attack/AttackParam.h"
#include "State/PlayerState.h"

class Camera;

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

	// 解放
	void Release(void) override;

	// アニメーションの取得
	AnimationController* GetAnimationController(void) const { return animationController_; }

	// 移動用の入力処理
	void MoveByInput(float speed);

	// ヘルスの取得
	Health& GetHealth(void) { return health_; }

	// 攻撃が当たりそうなときに呼ばれる
	void OnHit(int damage) override;

	// 倒れたか
	bool IsDead(void) const { return state_.GetStateId() == PLAYER_STATE::DEAD; }

	// 攻撃開始
	void StartAttack(ATTACK_ID id);

	// 攻撃終了
	void EndAttack(void);

	// 自分の攻撃が当たったときに呼ばれる
	void OnAttackHit(void);

	// 攻撃の判定
	const AttackController& GetAttackController(void) const { return attackController_; }

	// 回避開始
	void StartDodge(int invincibleFrame);

	// 向いている方向へ進む
	void MoveForward(float speed);

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
	AttackParam attackParams_[static_cast<int>(ATTACK_ID::MAX)];

private:

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
};
