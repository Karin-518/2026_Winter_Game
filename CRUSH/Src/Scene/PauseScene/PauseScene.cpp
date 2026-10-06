#include <DxLib.h>
#include "../../Application.h"
#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../../Common/Collision/Collision.h"
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

	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);


	if (InputManager::GetInstance()->IsActionUp(INPUT_INFO::ACTION::DECIDE))
	{
		if (Collision2D::HitPointRect(mouseX, mouseY, continueRect_))
		{
			InputManager::GetInstance()->SetMouseLock(true);
			SceneManager::GetInstance()->PopScene();
			return;
		}
		else if (Collision2D::HitPointRect(mouseX, mouseY, exitRect_))
		{
			Application::GetInstance()->SetEnd(true);
			return;
		}
	}
}

void PauseScene::Draw(void)
{
#ifdef _DEBUG
	DrawBox(0, 0, 1024, 640, 0x000000, true);
	DrawString(0, 0, "ポーズ画面", 0xffffff);

	DrawBox(continueRect_.left, continueRect_.top,
		continueRect_.right, continueRect_.bottom,
		GetColor(255, 255, 255), FALSE);
	DrawString(continueRect_.left + 20, continueRect_.top + 15,
		"続ける", GetColor(255, 255, 255));

	DrawBox(exitRect_.left, exitRect_.top,
		exitRect_.right, exitRect_.bottom,
		GetColor(255, 255, 255), FALSE);
	DrawString(exitRect_.left + 20, exitRect_.top + 15,
		"ゲーム終了", GetColor(255, 255, 255));

#endif //_DEBUG
}

void PauseScene::Release(void)
{
}
