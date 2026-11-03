#include "../../../Common/Math/Math.h"
#include "../../../Resource/ResourceManager.h"

#include "PlayerWeapon.h"

namespace
{
	// 武器の大きさ
	constexpr float WEAPON_SCALE = 1.5f;

	// 手から見た、武器の回転のずれ
	constexpr VECTOR OFFSET_ROT_DEG = { 90.0f, 90.0f, 0.0f };
	
	// 手から見た、武器の位置のずれ
	constexpr VECTOR OFFSET_POS = { -15.0f, 10.0f, -5.0f };
}

PlayerWeapon::PlayerWeapon(void)
	:
	modelId_(-1),
	ownerModelId_(-1),
	handFrameIndex_(-1)
{
}

PlayerWeapon::~PlayerWeapon(void)
{
}

void PlayerWeapon::Init(int ownerModelId, const TCHAR* handFrameName)
{
	modelId_ = ResourceManager::GetInstance()->LoadModelDuplicate(ResourceManager::SRC::PLAYER_SWORD);
	ownerModelId_ = ownerModelId;
	handFrameIndex_ = MV1SearchFrame(ownerModelId_, handFrameName);
}

void PlayerWeapon::Update(void)
{
	// 手のフレームが見つかっていない
	if (handFrameIndex_ == -1) return;

	// 手のボーンの、ワールドでの位置・回転・大きさ
	MATRIX handMatrix = MV1GetFrameLocalWorldMatrix(ownerModelId_, handFrameIndex_);

	// 先に武器のずれをかけてから、手の行列をかける
	MATRIX weaponMatrix = MMult(CalcOffsetMatrix(), handMatrix);

	MV1SetMatrix(modelId_, weaponMatrix);
}

void PlayerWeapon::Draw(void) const
{
	MV1DrawModel(modelId_);
}

MATRIX PlayerWeapon::CalcOffsetMatrix(void) const
{
	MATRIX scaleMatrix = MGetScale(VGet(WEAPON_SCALE, WEAPON_SCALE, WEAPON_SCALE));

	// X → Y → Z の順に回す
	MATRIX rotMatrix = MGetRotX(Math::Deg2Rad(OFFSET_ROT_DEG.x));
	rotMatrix = MMult(rotMatrix, MGetRotY(Math::Deg2Rad(OFFSET_ROT_DEG.y)));
	rotMatrix = MMult(rotMatrix, MGetRotZ(Math::Deg2Rad(OFFSET_ROT_DEG.z)));

	MATRIX translateMatrix = MGetTranslate(OFFSET_POS);

	// 大きさ → 回転 → 位置 の順にかける
	return MMult(MMult(scaleMatrix, rotMatrix), translateMatrix);
}