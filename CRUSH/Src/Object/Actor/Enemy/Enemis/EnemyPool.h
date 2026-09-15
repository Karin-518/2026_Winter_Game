#pragma once

#include <vector>
#include <stack>
#include <algorithm>

#include "EnemyFactory.h"

enum class PoolMode
{
	EXPANDABLE,		// 空なら new して増やす
	LIMITED,		// 上限を超える場合は生成拒否
	RECYCLE,		// 超える場合は最古の使用中オブジェクトを強制回収
};

class EnemyPool
{
public:
	EnemyPool(EnemyFactory* factory, PoolMode mode = PoolMode::EXPANDABLE);
	~EnemyPool();

	// 事前確保
	void PreAllocate(ENEMY_TYPE type, size_t count);

	// プールから借りる
	EnemyBase* Acquire(ENEMY_TYPE type);
	
	// プールに返却
	void Release(ENEMY_TYPE type, EnemyBase* enemy);

	// 全削除
	void Clear(void);

private:
	EnemyBase* ForceRecycle(ENEMY_TYPE type);

private:
	EnemyFactory* factory_;
	PoolMode mode_;

	// プール
	std::unordered_map<ENEMY_TYPE, std::stack<EnemyBase*>> pools_;

	// 事前確保した場合の最大数
	std::unordered_map<ENEMY_TYPE, size_t> maxSizes_;
};
