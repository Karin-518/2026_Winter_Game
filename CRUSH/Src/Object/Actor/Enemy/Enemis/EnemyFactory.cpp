#include "EnemyFactory.h"

// 各 Enemyクラス
#include "Enemy1/Enemy1.h"
#include "Enemy2/Enemy2.h"

// エネミー登録テーブル
namespace
{
	EnemyBase* CreateEnemy1() { return new Enemy1; }
	EnemyBase* CreateEnemy2() { return new Enemy2; }

	struct EnemyEntry
	{
		ENEMY_TYPE type;
		EnemyBase* (*creator)();
	};

	const EnemyEntry ENEMY_TABLE[] =
	{
		{ ENEMY_TYPE::ENEMY_1, &CreateEnemy1 },
		{ ENEMY_TYPE::ENEMY_2, &CreateEnemy2 },
	};
}

EnemyFactory::~EnemyFactory()
{
	// プロトタイプの削除
	for (auto& p : prototypes_)
	{
		delete p.second;
	}
	prototypes_.clear();
}

void EnemyFactory::Load()
{
	// 既にロード済みなら何もしない
	if (!prototypes_.empty()) return;

	// テーブルから登録しているエネミーのロードを行う
	for (const auto& entry : ENEMY_TABLE)
	{
		// エネミーの生成
		EnemyBase* proto = entry.creator();

		// リソースのロード
		proto->Load();

		// テンプレートに登録する
		prototypes_.emplace(entry.type, proto);
	}
}

EnemyBase* EnemyFactory::Create(const ENEMY_TYPE& id)
{
	auto it = prototypes_.find(id);

	// テンプレートが登録されていない
	if (it == prototypes_.end()) return nullptr;

	// 複製
	return it->second->Clone();
}
