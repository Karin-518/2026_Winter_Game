#pragma once
#include "EnemyInfo.h"

#include <DxLib.h>

class EnemyBase
{
public:
	virtual ~EnemyBase();

	// プロトタイプ
	virtual EnemyBase* Clone() const = 0;

	virtual void Load(void) = 0;		// リソースのロード(1回)
	virtual void Init(void) = 0;		// 初回生成時
	virtual void Reset(void) = 0;		// 再利用時
	virtual void Update(void) = 0;		// 更新
	virtual void Draw(void) = 0;		// 描画

	// 共通操作
	void SetPos(const VECTOR& pos) { pos_ = pos; }
	const VECTOR& GetPos()const { return pos_; }

	// 生存フラグ取得
	bool isAlive()const { return isAlive_; }

protected:
	// 共通データ
	ENEMY_TYPE type_;

	// モデル
	int modelHandle_ = -1;

	// 操作系
	VECTOR scale_;
	VECTOR rot_;
	VECTOR pos_;

	// フラグ
	bool isAlive_;
};
