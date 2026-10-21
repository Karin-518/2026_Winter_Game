#pragma once
#include "../ActorBase.h"
#include "../../Common/Health/Health.h"

class Player;

class Boss : public ActorBase
{
public:

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE,
		WALK,
		ATTACK_BASIC,

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

	// 攻撃が当たりそうなときに呼ばれる
	void OnHit(int damage) override;

	// 倒れたか
	bool IsDead(void) const { return health_.IsDead(); }

private:

	// プレイヤー
	Player* player_;

	// ヘルス
	Health health_;

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
};