#pragma once
#include <DxLib.h>

class BossFireball
{
public:

	// コンストラクタ
	BossFireball(const VECTOR& startPos, const VECTOR& moveDir, int damage);

	// デストラクタ
	~BossFireball(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void) const;

	// 生きているか
	bool IsAlive(void) const { return isAlive_; }

	// 座標の取得
	VECTOR GetPos(void) const { return pos_; }

	// 半径の取得
	float GetRadius(void) const { return radius_; }

	// ダメージを取得
	int GetDamage(void) const { return damage_; }

	// 向きの取得
	const VECTOR& GetMoveDir(void) const { return moveDir_; }

	// 当たったら消える
	void OnHit(void);

private:

	// 座標
	VECTOR pos_;

	// 進む向き
	VECTOR moveDir_;

	// 進む距離
	float speed_;

	// 半径
	float radius_;

	// 当たったときのダメージ量
	int damage_;

	// 残りの寿命
	int lifeFrame_;

	// まだ飛んでいるか
	bool isAlive_;
};