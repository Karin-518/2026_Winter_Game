#pragma once
#include "../ActorBase.h"
#include "../../Common/State/StateContext.h"
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

private:

	// カメラ
	Camera* camera_;

	// ステートを管理
	StateContext<Player, PLAYER_STATE> state_;

private:

	// リソースロード
	void InitLoad(void) override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void) override;
};
