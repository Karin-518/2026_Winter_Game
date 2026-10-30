#pragma once
#include "../SceneBase.h"
#include "../../UI/HpBar/HpBar.h"

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

	// ステージの当たり判定を、アクターとカメラに登録する
	void RegisterStageCollider(void);

	// バトルが終わったかどうか確認
	bool CheckBattleEnd(void);

	// プレイヤーの攻撃をボスに当てる
	void ResolvePlayerAttack(void);

	// ボスの攻撃をプレイヤーに当てる
	void ResolveBossAttack(void);
};
