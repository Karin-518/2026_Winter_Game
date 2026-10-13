#include <DxLib.h>
#include "../../Application.h"
#include "../../Input/InputManager.h"
#include "../../Camera/Camera.h"
#include "../../Object/Actor/ActorBase.h"
#include "../../Object/Actor/Player/Player.h"
#include "../../Object/Actor/Enemy/Boss.h"
#include "../../Object/Actor/Stage/Stage.h"
#include "../SceneManager.h"
#include "../PauseScene/PauseScene.h"

#include "GameScene.h"

GameScene::GameScene(void)
	:
	camera_(nullptr),
	stage_(nullptr)
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

	// 全てのアクターを初期化
	for (auto actor : allActor_)
	{
		// 初期化
		actor->Init();
	}
}

void GameScene::Load(void)
{
	// 生成処理
	camera_ = new Camera();					// カメラの生成
	stage_ = new Stage();					// ステージの生成
	Player* player = new Player(camera_);	// プレイヤーの生成
	Boss* boss = new Boss(player);			// 敵の生成

	// アクター配列に入れる
	allActor_.push_back(player);
	allActor_.push_back(boss);

	// カメラモード変更
	camera_->SetFollow(player);
	camera_->SetLookTarget(boss);
	camera_->ChangeMode(Camera::MODE::FOLLOW);

	// ステージの読み込み
	stage_->Load();

	// 全てのアクターを読み込み
	for (auto actor : allActor_)
	{
		// 読み込み
		actor->Load();
	}
}

void GameScene::LoadEnd(void)
{
	// カメラの初期化
	camera_->Init();

	// ステージ初期化
	stage_->LoadEnd();

	// 全てのアクターを読み込み後
	for (auto actor : allActor_)
	{
		// 読み込み
		actor->LoadEnd();
	}

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

	// 全てのアクターを回す
	for (auto actor : allActor_)
	{
		// 更新処理
		actor->Update();
	}

	// カメラの更新
	camera_->Update();
}

void GameScene::Draw(void)
{
	// カメラの描画更新
	camera_->SetBeforeDraw();

	// ステージ描画
	stage_->Draw();

	// 全てのアクターを回す
	for (auto actor : allActor_)
	{
		// 更新処理
		actor->Draw();
	}

	// デバッグ描画
	camera_->DrawDebug();
}

void GameScene::Release(void)
{
	// ステージ解放
	stage_->Release();
	delete stage_;
	
	// 全てのアクターを回す
	for (auto actor : allActor_)
	{
		// 更新処理
		actor->Release();
		delete actor;
	}

	// 配列をクリア
	allActor_.clear();

	// カメラの解放
	camera_->Release();
	delete camera_;
}

void GameScene::RegisterStageCollider(void)
{
	const ColliderModel* stageCollider = stage_->GetCollider();

	// アクターに登録
	for (auto actor : allActor_)
	{
		actor->AddGroundModel(stageCollider);
		actor->AddWallModel(stageCollider);
	}

	// カメラに登録(カメラは地面だけ)
	camera_->AddHitStage(stageCollider);
}