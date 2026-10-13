#include <DxLib.h>
#include "../../Application.h"
#include "../../Input/InputManager.h"
#include "../../Camera/Camera.h"
#include "../../Object/Actor/Player/Player.h"
#include "../../Object/Actor/Enemy/Boss.h"
#include "../../Object/Actor/Stage/Stage.h"
#include "../SceneManager.h"
#include "../PauseScene/PauseScene.h"
#include "../TitleScene/TitleScene.h"

#include "GameScene.h"

GameScene::GameScene(void)
	:
	camera_(nullptr),
	stage_(nullptr),
	player_(nullptr),
	boss_(nullptr)
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
		InputManager::GetInstance()->SetMouseLock(false);
		SceneManager::GetInstance()->PushScene(std::make_shared<PauseScene>());
		return;
	}

	// ステージ更新
	stage_->Update();

	// プレイヤー更新
	player_->Update();

	// ボス更新
	boss_->Update();

	// カメラの更新
	camera_->Update();

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
		SceneManager::GetInstance()->ChangeScene(std::make_shared<TitleScene>());
		return true;
	}

	if (!player_->IsDead()) return false;

	SceneManager::GetInstance()->ChangeScene(std::make_shared<TitleScene>());
	return true;
}
