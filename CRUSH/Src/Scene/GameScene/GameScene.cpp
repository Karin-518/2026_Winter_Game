#include <DxLib.h>
#include "../../Application.h"
#include "../../Audio/AudioManager.h"
#include "../../Input/InputManager.h"
#include "../../Camera/Camera.h"
#include "../../Object/Actor/Player/Player.h"
#include "../../Object/Actor/Enemy/Boss.h"
#include "../../Object/Actor/Stage/Stage.h"
#include "../../Object/Common/Attack/AttackParam.h"
#include "../../Object/Common/Attack/AttackController.h"
#include "../SceneManager.h"
#include "../PauseScene/PauseScene.h"
#include "../TitleScene/TitleScene.h"
#include "../GameOver/GameOver.h"
#include "../GameClear/GameClear.h"

#include "GameScene.h"

GameScene::GameScene(void)
	:
	camera_(nullptr),
	stage_(nullptr),
	player_(nullptr),
	boss_(nullptr),
	playerHpBar_(20, 20, 300, 20, 0x00ff00),
	bossHpBar_(200, 600, 600, 16, 0xff0000)
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// カメラの初期化
	camera_->Init();

	// ステージ初期化
	stage_->Init();

	// プレイヤー初期化
	player_->Init();

	// ボス初期化
	boss_->Init();
}

void GameScene::Load(void)
{
	// サウンド読みこみ
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME);

	// 生成処理
	camera_ = new Camera();				// カメラの生成
	stage_ = new Stage();				// ステージの生成
	player_ = new Player(camera_);		// プレイヤーの生成
	boss_ = new Boss(player_);			// 敵の生成

	// カメラモード変更
	camera_->SetFollow(player_);
	camera_->SetLookTarget(boss_);
	camera_->ChangeMode(Camera::MODE::FOLLOW);

	// ステージの読み込み
	stage_->Load();

	// プレイヤーの読み込み
	player_->Load();

	// ボスの読み込み
	boss_->Load();
}

void GameScene::LoadEnd(void)
{
	// カメラの初期化
	camera_->Init();

	// ステージ初期化
	stage_->LoadEnd();

	// プレイヤー初期化
	player_->LoadEnd();

	// ボス初期化
	boss_->LoadEnd();

	// ステージの当たり判定ができた後に登録する
	RegisterStageCollider();
}

void GameScene::Update(void)
{
	if (InputManager::GetInstance()->IsActionUp(INPUT_INFO::ACTION::PAUSE))
	{
		// ポーズシーンへ
		SceneManager::GetInstance()->PushScene(std::make_shared<PauseScene>());
		return;
	}

	// ステージ更新
	stage_->Update();

	// プレイヤー更新
	player_->Update();

	// ボス更新
	boss_->Update();

	// 攻撃の命中
	ResolvePlayerAttack();
	ResolveBossAttack();

	// カメラの更新
	camera_->Update();

	playerHpBar_.Update(player_->GetHealth().GetHp(), player_->GetHealth().GetMaxHp());

	bossHpBar_.Update(boss_->GetHealth().GetHp(), boss_->GetHealth().GetMaxHp());

	// 勝敗がついていたら、何もしない
	if (CheckBattleEnd()) return;
}

void GameScene::Draw(void)
{
	// カメラの描画更新
	camera_->SetBeforeDraw();

	// ステージ描画
	stage_->Draw();

	// プレイヤー描画
	player_->Draw();

	// ボス描画
	boss_->Draw();

	// デバッグ描画
	camera_->DrawDebug();

	playerHpBar_.Draw();
	bossHpBar_.Draw();
	DrawString(200, 580, "BOSS", 0xffffff);
}

void GameScene::Release(void)
{
	// ボス解放
	boss_->Release();
	delete boss_;
	boss_ = nullptr;

	// プレイヤー解放
	player_->Release();
	delete player_;
	player_ = nullptr;

	// ステージ解放
	stage_->Release();
	delete stage_;
	stage_ = nullptr;

	// カメラの解放
	camera_->Release();
	delete camera_;
	camera_ = nullptr;

	// 音の解放
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME);
}

void GameScene::RegisterStageCollider(void)
{
	const ColliderModel* stageCollider = stage_->GetCollider();

	// プレイヤーに登録
	player_->AddGroundModel(stageCollider);
	player_->AddWallModel(stageCollider);

	// ボスに登録
	boss_->AddGroundModel(stageCollider);
	boss_->AddWallModel(stageCollider);

	// カメラに登録(カメラは地面だけ)
	camera_->AddHitStage(stageCollider);
}

bool GameScene::CheckBattleEnd(void)
{
	// ボスを先に見る
	// 相打ちはクリア判定
	if (boss_->IsDead())
	{
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameClear>());
		return true;
	}

	if (!player_->IsDead()) return false;

	SceneManager::GetInstance()->ChangeScene(std::make_shared<GameOver>());
	return true;
}

void GameScene::ResolvePlayerAttack(void)
{
	const AttackController& attack = player_->GetAttackController();

	// 判定区域外、または1振りで当たった
	if (!attack.CanHit()) return;

	const ColliderPart* hitPart = boss_->FindHitPart(
		attack, player_->GetPos(), player_->GetForward());
	if (hitPart == nullptr) return;

	// ダメージを与えて、当たったことを記憶
	boss_->OnHit(attack.GetParam()->damage);
	player_->OnAttackHit();
}

void GameScene::ResolveBossAttack(void)
{
	const AttackController& attack = boss_->GetAttackController();

	// 判定区域外、または1振りで当たった
	if (!attack.CanHit()) return;

	// 判定の形とプレイヤーの円が重なっていない
	if (!attack.IsHit(boss_->GetPos(), boss_->GetForward(),
		player_->GetPos(), player_->GetBodyRadius())) return;

	// ダメージを与えて、当たったことを記憶
	player_->OnHit(attack.GetParam()->damage);
	boss_->OnAttackHit();
}