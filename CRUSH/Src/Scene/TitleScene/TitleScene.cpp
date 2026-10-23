#include <DxLib.h>
#include "../../Application.h"
#include "../../Resource/ResourceManager.h"
#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../GameScene/GameScene.h"
#include "../Confirm/ConfirmScene.h"

#include "TitleScene.h"

TitleScene::TitleScene(void)
	:
	handle_(-1),
	alpha_(0.0f),
	isIncreasing_(false)
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	alpha_ = 255.0f;
	isIncreasing_ = false;
}

void TitleScene::Load(void)
{
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::TITLE);
	handle_ = ResourceManager::GetInstance()->Load(ResourceManager::SRC::TITLE_LOGO).GetHandleId();
}

void TitleScene::LoadEnd(void)
{
	Init();
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_TITLE);
}

void TitleScene::Update(void)
{
	// Esc を押したら終了確認へ
	if (InputManager::GetInstance()->IsActionUp(INPUT_INFO::ACTION::CANCEL))
	{
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_CANCEL);
		SceneManager::GetInstance()->PushScene(std::make_shared<ConfirmScene>());
		return;
	}

	// スペースが押されると次のシーンへ
	if(InputManager::GetInstance()->IsActionUp(INPUT_INFO::ACTION::DECIDE))
	{
		// ゲームシーンへ
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_DECIDE);
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameScene>());
		return;
	}

	// ボタンのアルファ値を変化させる
	if (isIncreasing_)
	{
		alpha_ += 5.0f;

		// 増加が最大になったら減少に切り替える
		if (alpha_ >= 255.0f)
		{
			alpha_ = 255.0f;
			isIncreasing_ = false;
		}
	}
	else
	{
		alpha_ -= 5.0f; // 減少速度

		// 減少が最小になったら増加に切り替える
		if (alpha_ <= 0.0f)
		{
			alpha_ = 0.0f;
			isIncreasing_ = true;
		}
	}
}

void TitleScene::Draw(void)
{
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 2.0, 0.0, handle_, true);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_));
	DrawString(400, 500, "Press Space to Start", 0xffffff,true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void TitleScene::Release(void)
{
	DeleteGraph(handle_);
}
