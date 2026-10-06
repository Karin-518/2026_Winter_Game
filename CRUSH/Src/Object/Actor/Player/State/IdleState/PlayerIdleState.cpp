#include "../../../../../Input/InputManager.h"
#include "../../../../Common/AnimationController/AnimationController.h"
#include "../../Player.h"
#include "PlayerIdleState.h"

void PlayerIdleState::Enter(Player& owner)
{
	// 待機アニメーションを再生
	owner.GetAnimationController()->Play(static_cast<int>(Player::ANIM_TYPE::IDLE));
}

PLAYER_STATE PlayerIdleState::Update(Player& owner)
{
	// 移動入力があれば、移動ステートに切り替え
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_UP) ||
	   InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_DOWN) ||
	   InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_LEFT) ||
	   InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_RIGHT))
	{
		return PLAYER_STATE::MOVE;
	}

	// 何もなければ、切り替えない
	return PLAYER_STATE::NONE;
}