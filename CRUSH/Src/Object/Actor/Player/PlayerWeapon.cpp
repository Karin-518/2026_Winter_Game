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

	// 剣のモデルの中での、刃の根元の位置（仮の値。デバッグ描画を見て合わせる）
	constexpr VECTOR BLADE_ROOT_LOCAL_POS = { 0.0f, 10.0f, 0.0f };

	// 剣のモデルの中での、刃の先端の位置（仮の値。デバッグ描画を見て合わせる）
	constexpr VECTOR BLADE_TIP_LOCAL_POS = { 0.0f, 90.0f, 0.0f };

	// 刃のデバッグ描画の色
	constexpr unsigned int BLADE_DEBUG_COLOR = 0x00ffff;

	// 刃の両端に描く球の半径
	constexpr float BLADE_DEBUG_SPHERE_RADIUS = 3.0f;

	// 刃の両端に描く球の分割数
	constexpr int BLADE_DEBUG_SPHERE_DIV_NUM = 8;
}

PlayerWeapon::PlayerWeapon(void)
	:
	modelId_(-1),
	ownerModelId_(-1),
	handFrameIndex_(-1),
	bladeRootPos_({}),
	bladeTipPos_({})
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

	// 剣のモデルの中の位置を、武器と同じ行列でワールド座標に直す
	bladeRootPos_ = VTransform(BLADE_ROOT_LOCAL_POS, weaponMatrix);
	bladeTipPos_ = VTransform(BLADE_TIP_LOCAL_POS, weaponMatrix);
}

void PlayerWeapon::Draw(void) const
{
	MV1DrawModel(modelId_);
}

void PlayerWeapon::DrawDebug(void) const
{
	// 刃の芯の線と、両端の球（根元と先端の位置合わせ用）
	DrawLine3D(bladeRootPos_, bladeTipPos_, BLADE_DEBUG_COLOR);

	DrawSphere3D(bladeRootPos_, BLADE_DEBUG_SPHERE_RADIUS, BLADE_DEBUG_SPHERE_DIV_NUM,
		BLADE_DEBUG_COLOR, BLADE_DEBUG_COLOR, true);

	DrawSphere3D(bladeTipPos_, BLADE_DEBUG_SPHERE_RADIUS, BLADE_DEBUG_SPHERE_DIV_NUM,
		BLADE_DEBUG_COLOR, BLADE_DEBUG_COLOR, true);
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