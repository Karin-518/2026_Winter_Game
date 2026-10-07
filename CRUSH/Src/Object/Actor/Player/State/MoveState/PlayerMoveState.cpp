#include "../../../../../Input/InputManager.h"
#include "../../../../Common/AnimationController/AnimationController.h"
#include "../../Player.h"
#include "PlayerMoveState.h"

namespace
{
	// 歩き移動速度
	const float WALK_POW = 5.0f;

	// 走り移動速度
	const float SPRINT_POW = 10.0f;
}

void PlayerMoveState::Enter(Player& owner)
{
	// 歩くアニメーションの再生
	owner.GetAnimationController()->Play(static_cast<int>(Player::ANIM_TYPE::WALK));
}

PLAYER_STATE PlayerMoveState::Update(Player& owner)
{
	// 移動入力がなければ、待機ステートに切り替え
	if (!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_UP) &&
		!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_DOWN) &&
		!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_LEFT) &&
		!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_RIGHT))
	{
		return PLAYER_STATE::IDLE;
	}
	
	// 走る入力があれば、移動速度を上げる
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::SPRINT))
	{
		owner.MoveByInput(SPRINT_POW);
	}
	else
	{
		owner.MoveByInput(WALK_POW);
	}

	// 何もなければ、切り替えない
	return PLAYER_STATE::NONE;
}
