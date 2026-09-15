#include "Enemy1.h"
#include "../../../../../Common/Math/Math.h"

Enemy1::Enemy1(const Enemy1& rhs)
{
	// モデルの複製
	if (rhs.modelHandle_ != -1)
	{
		modelHandle_ = MV1DuplicateModel(rhs.modelHandle_);
	}
}

void Enemy1::Load(void)
{
	// オリジナルロード(1回のみ)
	modelHandle_ = MV1LoadModel("Data/Model/Player/Player.mv1");
}

void Enemy1::Init(void)
{
	// 固定パラメータ
	type_ = ENEMY_TYPE::ENEMY_1;
}

void Enemy1::Reset(void)
{
	MV1SetPosition(modelHandle_, Math::VECTOR_ZERO);
}

void Enemy1::Update(void)
{
	// 更新処理
}

void Enemy1::Draw(void)
{
	// 描画処理
	MV1DrawModel(modelHandle_);
}
