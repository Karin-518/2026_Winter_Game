#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../../Common/Collision/Collision.h"
#include "../SceneManager.h"
#include "../TitleScene/TitleScene.h"
#include "GameClear.h"

GameClear::GameClear(void)
	:
	handle_(-1),
	alpha_(0.0f),
	isIncreasing_(false)
{
}

GameClear::~GameClear(void)
{
}

void GameClear::Init(void)
{
	alpha_ = 255.0f;
	isIncreasing_ = false;
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

void GameClear::Draw(void)
{
	DrawBox(0, 0, 1024, 640, 0x000000, true);
	DrawString(420, 200, "ゲームクリア", 0xffffff);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_));
	DrawString(400, 500, "Press Space to Title", 0xffffff, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameClear::Release(void)
{
}
