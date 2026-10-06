#include <DxLib.h>
#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../GameScene/GameScene.h"
#include "PauseScene.h"

PauseScene::PauseScene(void)
{
	handle_ = -1;
}

PauseScene::~PauseScene(void)
{
}

void PauseScene::Init(void)
{
}

void PauseScene::Load(void)
{
}

void PauseScene::LoadEnd(void)
{
	Init();
}

void PauseScene::Update(void)
{
	if (InputManager::GetInstance()->IsActionUp(INPUT_INFO::ACTION::PAUSE))
	{
		// ゲームシーンへ
		InputManager::GetInstance()->SetMouseLock(true);
		SceneManager::GetInstance()->PopScene();
		return;
	}
}

void PauseScene::Draw(void)
{
#ifdef _DEBUG
	DrawBox(0, 0, 1024, 640, 0x000000, true);
	DrawString(0, 0, "ポーズ画面", 0xffffff);
#endif //_DEBUG
}

void PauseScene::Release(void)
{
}
