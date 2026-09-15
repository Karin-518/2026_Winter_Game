#pragma once

#include <unordered_map>

#include "EnemyBase.h"

class EnemyFactory
{
public:
	EnemyFactory() = default;
	~EnemyFactory();

	// リソースのロード(起動時に一回だけ)
	void Load();

	// エネミーを作成する(複製)
	EnemyBase* Create(const ENEMY_TYPE& id);

private:
	// プロトタイプ(ロード済みのエネミーを格納)
	std::unordered_map<ENEMY_TYPE, EnemyBase*> prototypes_;

	// コピー禁止
	EnemyFactory(const EnemyFactory&) = delete;
	EnemyFactory& operator=(const EnemyFactory&) = delete;
};
