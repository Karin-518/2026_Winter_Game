#include "../../../../Input/InputManager.h"
#include "../../../Common/AnimationController/AnimationController.h"
#include "../Player.h"
#include "PlayerMoveState.h"

namespace
{
	// 歩き移動速度
	const float WALK_POW = 6.0f;

	// 走り移動速度
	const float SPRINT_POW = 12.0f;
}

void PlayerMoveState::Enter(Player& owner)
{
	// 歩くアニメーションの再生
	owner.GetAnimationController()->Play(static_cast<int>(Player::ANIM_TYPE::WALK));
}

PLAYER_STATE PlayerMoveState::Update(Player& owner)
{
	// 攻撃入力があれば、攻撃ステートに切り替え
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::ATTACK))
	{
		return PLAYER_STATE::ATTACK;
	}

	// 回避入力があれば、回避ステートに切り替え
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::DODGE))
	{
		return PLAYER_STATE::DODGE;
	}

	// 移動入力がなければ、待機ステートに切り替え
	if (!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_UP) &&
		!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_DOWN) &&
		!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_LEFT) &&
		!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_RIGHT))
	{
		return PLAYER_STATE::IDLE;
	}
	
	// 歩きをデフォルトにしておく
	Player::ANIM_TYPE anim = Player::ANIM_TYPE::WALK;
	float pow = WALK_POW;

	// 走る入力があれば、移動速度を上げる
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::SPRINT))
	{
		anim = Player::ANIM_TYPE::RUN;
		pow = SPRINT_POW;
	}

	// アニメーションの再生と移動
	owner.GetAnimationController()->Play(static_cast<int>(anim));
	owner.MoveByInput(pow);

	// 何もなければ、切り替えない
	return PLAYER_STATE::NONE;
}
