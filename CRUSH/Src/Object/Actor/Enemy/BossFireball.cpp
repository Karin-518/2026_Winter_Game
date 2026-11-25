#include "../../../Common/Math/Math.h"

#include "BossFireball.h"

namespace
{
	// 移動速度
	constexpr float SPEED = 30.0f;

	// 半径
	constexpr float RADIUS = 80.0f;

	// 消えるまでのフレーム数
	constexpr int LIFE_FRAME = 120;

	// デバッグ表示の球の分割数
	constexpr int DEBUG_SPHERE_DIV_NUM = 8;

	// デバッグ表示の色
	constexpr unsigned int DEBUG_COLOR = 0xff8000;

	// 地面の高さ
	constexpr float GROUND_HEIGHT = 0.0f;
}

BossFireball::BossFireball(const VECTOR& startPos, const VECTOR& moveDir, int damage)
	:
	pos_(startPos),
	moveDir_({}),
	speed_(SPEED),
	radius_(RADIUS),
	damage_(damage),
	lifeFrame_(LIFE_FRAME),
	isAlive_(true)
{
	// 向きがなければ、すぐ消す
	if (Math::EqualsVZero(moveDir))
	{
		isAlive_ = false;
		return;
	}
	moveDir_ = VNorm(moveDir);
}

BossFireball::~BossFireball(void)
{
}

void BossFireball::Update(void)
{
	// 消えていたら何もしない
	if (!isAlive_) return;

	// 移動させる
	VECTOR moveAmount = VScale(moveDir_, speed_);
	pos_ = VAdd(pos_, moveAmount);

	// 地面より下に行ったら消える
	if (pos_.y < GROUND_HEIGHT)
	{
		isAlive_ = false;
		return;
	}

	// 寿命を減らす
	lifeFrame_--;

	// 寿命が残っていれば、まだ飛ばす
	if (lifeFrame_ > 0) return;

	// 寿命が切れたら消える
	isAlive_ = false;
}

void BossFireball::Draw(void) const
{
	// 消えていたら何もしない
	if (!isAlive_) return;

	// 仮の見た目
	DrawSphere3D(pos_, radius_, DEBUG_SPHERE_DIV_NUM, DEBUG_COLOR, DEBUG_COLOR, false);
}

void BossFireball::OnHit(void)
{
	// 当たったら消える
	isAlive_ = false;
}