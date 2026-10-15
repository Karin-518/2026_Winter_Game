#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../../Common/Collision/Collision.h"
#include "../SceneManager.h"
#include "../TitleScene/TitleScene.h"
#include "GameOver.h"

GameOver::GameOver(void)
	:
	handle_(-1)
{
}

GameOver::~GameOver(void)
{
}

void GameOver::Init(void)
{
}

void GameOver::Load(void)
{
}

void GameOver::LoadEnd(void)
{
	Init();
}

void GameOver::Update(void)
{
	if (InputManager::GetInstance()->IsActionUp(INPUT_INFO::ACTION::DECIDE))
	{
		// タイトルへ
		SceneManager::GetInstance()->JumpScene(std::make_shared<TitleScene>());
		return;
	}
}

void GameOver::Draw(void)
{
	DrawBox(0, 0, 1024, 640, 0x000000, true);
	DrawString(10, 10, "ゲームオーバー", 0xffffff);
}

void GameOver::Release(void)
{
}
