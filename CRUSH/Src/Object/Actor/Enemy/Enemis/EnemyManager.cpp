#include "EnemyManager.h"

EnemyManager::EnemyManager(EnemyPool* pool)
	: pool_(pool)
{
}

EnemyManager::~EnemyManager()
{}

EnemyBase* EnemyManager::Spawn(ENEMY_TYPE type)
{
	// プールからエネミーを取り出す
	EnemyBase* enemy = pool_->Acquire(type);
	if (!enemy) return nullptr;

	// 配列に追加
	enemies.push_back(enemy);
	return enemy;
}

void EnemyManager::Update(void)
{
	// 全ての敵を検索
	for (auto& enemy : enemies)
	{
		// 更新
		enemy->Update();
	}
}

void EnemyManager::Draw(void)
{
	// 全ての敵を検索
	for (auto& enemy : enemies)
	{
		// 描画
		enemy->Draw();
	}
}

void EnemyManager::Release(void)
{
	for (auto enemy : enemies)
	{
		pool_->Release(ENEMY_TYPE::ENEMY_1, enemy);
	}

	// 配列をクリア
	enemies.clear();
}

