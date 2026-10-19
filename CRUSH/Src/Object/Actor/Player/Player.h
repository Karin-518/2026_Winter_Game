#pragma once
#include "../ActorBase.h"
#include "../../Common/State/StateContext.h"
#include "../../Common/Health/Health.h"
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

private:

	// カメラ
	Camera* camera_;

	// ヘルス
	Health health_;

	// ステートを管理
	StateContext<Player, PLAYER_STATE> state_;

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
};
