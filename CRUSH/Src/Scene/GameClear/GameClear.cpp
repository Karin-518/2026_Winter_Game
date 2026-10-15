#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../../Common/Collision/Collision.h"
#include "../SceneManager.h"
#include "../TitleScene/TitleScene.h"
#include "GameClear.h"

GameClear::GameClear(void)
	:
	handle_(-1)
{
}

GameClear::~GameClear(void)
{
}

void GameClear::Init(void)
{
}

void GameClear::Load(void)
{
}

void GameClear::LoadEnd(void)
{
	Init();
}

void GameClear::Update(void)
{
	if (InputManager::GetInstance()->IsActionUp(INPUT_INFO::ACTION::DECIDE))
	{
		// タイトルへ
		SceneManager::GetInstance()->JumpScene(std::make_shared<TitleScene>());
		return;
	}
}

void GameClear::Draw(void)
{
	DrawBox(0, 0, 1024, 640, 0x000000, true);
	DrawString(10, 10, "ゲームクリア", 0xffffff);
}

void GameClear::Release(void)
{
}
