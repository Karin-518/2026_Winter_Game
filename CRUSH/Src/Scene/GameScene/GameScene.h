#pragma once
#include "../SceneBase.h"
#include "../../UI/HpBar/HpBar.h"
#include "../../Object/Collider/CollisionManager.h"

class Camera;
class Stage;
class Player;
class Boss;

class GameScene : public SceneBase
{
public:

	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void) override;

	// 初期化
	void Init(void) override;

	// 読み込み
	void Load(void) override;
	
	// 読み込み後の処理
	void LoadEnd(void) override;
	
	// 更新
	void Update(void) override;
	
	// 描画
	void Draw(void) override;
	
	// 解放
	void Release(void) override;

private:

	// カメラ
	Camera* camera_;

	// ステージ
	Stage* stage_;

	// プレイヤー
	Player* player_;

	// ボス
	Boss* boss_;

	// プレイヤーのHPバー
	HpBar playerHpBar_;

	// ボスのHPバー
	HpBar bossHpBar_;

	// 攻撃と食らい判定の当たり
	CollisionManager collisionManager_;

	// ステージの当たり判定を、アクターとカメラに登録する
	void RegisterStageCollider(void);

	// バトルが終わったかどうか確認
	bool CheckBattleEnd(void);

	// 攻撃と食らい判定を登録
	void RegisterCollision(void);
};