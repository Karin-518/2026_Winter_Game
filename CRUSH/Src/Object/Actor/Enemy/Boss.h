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

private:

	// プレイヤー
	Player* player_;

	// ヘルス
	Health health_;

	// 検知フラグ(視野)
	bool isNoticeView_;

	// 検知フラグ(聴覚)
	bool isNoticeHearing_;

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

	// 視野描画
	void DrawViewRange(void);

	// 索敵
	void Search(void);

	// HPが0になったときに呼ぶ
	void OnDead(void);
};