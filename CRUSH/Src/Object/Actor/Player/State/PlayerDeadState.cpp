#include "PlayerDeadState.h"

void PlayerDeadState::Enter(Player& owner)
{
}

PLAYER_STATE PlayerDeadState::Update(Player& owner)
{
    return PLAYER_STATE::NONE;
}
