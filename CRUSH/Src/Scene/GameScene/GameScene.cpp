#include <DxLib.h>

#include "../../Application.h"
#include "../../Camera/Camera.h"
#include "../../Object/Actor/ActorBase.h"
#include "../../Object/Actor/Player/Player.h"
#include "../../Object/Actor/Enemy/Enemy.h"
#include "../../Object/Actor/Enemy/Enemis/EnemyFactory.h"
#include "../../Object/Actor/Enemy/Enemis/EnemyPool.h"
#include "../../Object/Actor/Enemy/Enemis/EnemyManager.h"
#include "../../Object/Actor/Stage/Stage.h"

#include "GameScene.h"

namespace
{
	// 線分のオフセット
	constexpr float LINE_OFFSET = 10.0f;

	// ポリゴンの検索回数
	constexpr int POLYGON_SEARCH_COUNT = 10;

	// 法線の移動量
	constexpr float NORMAL_MOVE_AMOUNT = 1.0f;
}

GameScene::GameScene(void)
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
	Enemy* enemy = new Enemy(player);		// 敵の生成

	// アクター配列に入れる
	allActor_.push_back(player);
	allActor_.push_back(enemy);

	// カメラモード変更
	camera_->SetFollow(player);
	camera_->ChangeMode(Camera::MODE::FOLLOW);

	// ステージの読み込み
	stage_->Load();

	// 全てのアクターを読み込み
	for (auto actor : allActor_)
	{
		// 読み込み
		actor->Load();
	}

	// 敵のファクトリー登録と読み込み
	enemyFac_ = new EnemyFactory();
	enemyFac_->Load();

	// 敵のプールを作成
	enemyPool_ = new EnemyPool(enemyFac_);
	enemyPool_->PreAllocate(ENEMY_TYPE::ENEMY_1, 10);
	enemyPool_->PreAllocate(ENEMY_TYPE::ENEMY_2, 10);

	// 敵のプールをマネージャーに登録
	enemyMng_ = new EnemyManager(enemyPool_);
	enemyMng_->Spawn(ENEMY_TYPE::ENEMY_1);
	enemyMng_->Spawn(ENEMY_TYPE::ENEMY_2);
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
}

void GameScene::Update(void)
{
	// カメラの更新
	camera_->Update();

	// ステージ更新
	stage_->Update();

	// 全てのアクターを回す
	for (auto actor : allActor_)
	{
		// 更新処理
		actor->Update();

		// 当たり判定を取るか？
		if (actor)
		{
			// 当たり判定
			FieldCollision(actor);
			WallCollision(actor);
		}
	}

	enemyMng_->Update();
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

	enemyMng_->Draw();

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

	// 敵の解放
	enemyMng_->Release();
	delete enemyMng_;
	delete enemyPool_;
	delete enemyFac_;
}

// ステージの床とプレイヤーの衝突
void GameScene::FieldCollision(ActorBase* actor)
{
	// 座標を所得
	VECTOR actorPos = actor->GetPos();

	// 線分の上座標
	VECTOR startPos = actorPos;
	startPos.y = actorPos.y + LINE_OFFSET;

	// 線分の下座標
	VECTOR endPos = actorPos;
	endPos.y = actorPos.y - LINE_OFFSET;

	// ステージのモデルを取得
	int modelId = stage_->GetModelId();

	// 線分とモデルの衝突判定
	MV1_COLL_RESULT_POLY res =
		MV1CollCheck_Line(modelId, -1, startPos, endPos);

	// ステージに当たっているか？
	if (res.HitFlag)
	{
		// 当たった場所に戻す
		actor->CollisionStage(res.HitPosition);
	}
}

void GameScene::WallCollision(ActorBase* actor)
{
	// 座標を取得
	VECTOR pos = actor->GetPos();

	// カプセルの座標
	VECTOR capStartPos = VAdd(pos, actor->GetStartCapsulePos());
	VECTOR capEndPos = VAdd(pos, actor->GetEndCapsulePos());

	// カプセルとの当たり判定
	auto hits = MV1CollCheck_Capsule
	(
		stage_->GetModelId(),			// ステージのモデルID
		-1,								// ステージ全てのポリゴンを指定
		capStartPos,					// カプセルの上
		capEndPos,						// カプセルの下
		actor->GetCapsuleRadius()		// カプセルの半径
	);

	// 衝突したポリゴン全ての検索
	for (int i = 0; i < hits.HitNum; i++)
	{
		// ポリゴンを1枚に分割
		auto hit = hits.Dim[i];

		// ポリゴン検索を制限(全てを検索すると重いため)
		for (int tryCnt = 0; tryCnt < POLYGON_SEARCH_COUNT; tryCnt++)
		{
			// 最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
			int pHit = HitCheck_Capsule_Triangle
			(
				capStartPos,					// カプセルの上
				capEndPos,						// カプセルの下
				actor->GetCapsuleRadius(),		// カプセルの半径
				hit.Position[0],				// ポリゴン1
				hit.Position[1],				// ポリゴン2
				hit.Position[2]					// ポリゴン3
			);

			// カプセルとポリゴンが当たっていた
			if (pHit)
			{
				// 当たっていたので座標をポリゴンの法線方向に移動させる
				pos = VAdd(pos, VScale(hit.Normal, NORMAL_MOVE_AMOUNT));

				// 球体の座標も移動させる
				capStartPos = VAdd(capStartPos, VScale(hit.Normal, NORMAL_MOVE_AMOUNT));
				capEndPos = VAdd(capEndPos, VScale(hit.Normal, NORMAL_MOVE_AMOUNT));

				// 複数当たっている可能性があるので再検索
				continue;
			}
		}
	}
	// 検出したポリゴン情報の後始末
	MV1CollResultPolyDimTerminate(hits);

	// 計算した場所にアクターを戻す
	actor->CollisionStage(pos);
}
