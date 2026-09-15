#pragma once
#include <memory>

#include "EnemyPool.h"

class EnemyManager
{
public:
	// コンストラクタ・デストラクタ
	EnemyManager(EnemyPool* pool);
	~EnemyManager();

	EnemyBase* Spawn(ENEMY_TYPE type);

	void Update(void);		// 更新
	void Draw(void);		// 描画
	void Release(void);		// 解放

private:
	EnemyPool* pool_;
	std::list<EnemyBase*> enemies;
};
