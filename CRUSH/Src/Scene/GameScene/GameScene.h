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
	GameScene(void);				// コンストラクタ
	~GameScene(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:

	// カメラ
	Camera* camera_;

	// ステージ
	Stage* stage_;

	// プレイヤー
	Player* player_;

	// ボス
	Boss* boss_;

	HpBar playerHpBar_;

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
