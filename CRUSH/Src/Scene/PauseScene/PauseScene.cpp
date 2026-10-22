#include "../../Application.h"
#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../../Common/Collision/Collision.h"
#include "../SceneManager.h"
#include "../GameScene/GameScene.h"
#include "../Confirm/ConfirmScene.h"

#include "PauseScene.h"

namespace
{
	//CONTINUE
	constexpr RECT CONTINUE_RECT = { 250, 200, 500, 260 };

	// EXIT
	constexpr RECT EXIT_RECT = { 250, 290, 500, 350 };

	// 無効なハンドル
	constexpr int INVALID_HANDLE = -1;

	// デバッグ表示の色
	constexpr unsigned int TEXT_COLOR = 0xffffff;
	constexpr unsigned int SELECT_COLOR = 0xffffff;
	constexpr unsigned int UNSELECT_COLOR = 0x808080;

	// デバッグ表示の文字位置
	constexpr int TEXT_OFFSET_X = 20;
	constexpr int TEXT_OFFSET_Y = 15;

	// デバッグ表示用の項目名
	const char* GetMenuLabel(PauseScene::MENU_TYPE type)
	{
		switch (type)
		{
		case PauseScene::MENU_TYPE::CONTINUE:	return "続ける";
		case PauseScene::MENU_TYPE::EXIT:		return "ゲーム終了";
		default:								return "";
		}
	}
}

PauseScene::PauseScene(void)
	:
	selectIndex_(0),
	menuButtons_{}
{
}

PauseScene::~PauseScene(void)
{
}

void PauseScene::Init(void)
{
	selectIndex_ = 0;

	// ポーズ中はマウスカーソルを動かせるようにする
	InputManager::GetInstance()->SetMouseLock(false);
}

void PauseScene::Load(void)
{
	// 二重に読み込まれてもボタンが増えないようにする
	menuButtons_.clear();

	// ボタンの追加
	AddButton(MENU_TYPE::CONTINUE, INVALID_HANDLE, CONTINUE_RECT);
	AddButton(MENU_TYPE::EXIT, INVALID_HANDLE, EXIT_RECT);
}

void PauseScene::LoadEnd(void)
{
	Init();
}

void PauseScene::Update(void)
{
	// ポーズ中にポーズボタンを押したら
	if (InputManager::GetInstance()->IsActionUp(INPUT_INFO::ACTION::PAUSE))
	{
		// ゲームシーンへ
		Execute(MENU_TYPE::CONTINUE);
		return;
	}

	// 選択処理
	UpdateSelect();

	// 選択した項目を実行
	Decide();
}

void PauseScene::Draw(void)
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	DrawString(10, 10, "ポーズ画面", 0xffffff);

	// 項目の描画（画像ができるまでは枠と文字で表示）
	for (const auto& button : menuButtons_)
	{
		DrawBox(button.rect.left, button.rect.top,
			button.rect.right, button.rect.bottom,
			UNSELECT_COLOR, FALSE);

		DrawString(button.rect.left + TEXT_OFFSET_X, button.rect.top + TEXT_OFFSET_Y,
			GetMenuLabel(button.menuType), TEXT_COLOR);
	}

	if (menuButtons_.empty()) return;

	// 選択中の項目の枠を白で上書きする
	const RECT& selectRect = menuButtons_[selectIndex_].rect;
	DrawBox(selectRect.left, selectRect.top,
		selectRect.right, selectRect.bottom,
		SELECT_COLOR, FALSE);
}

void PauseScene::Release(void)
{
	menuButtons_.clear();
}

void PauseScene::AddButton(MENU_TYPE type, int handle, const RECT& rect)
{
	menuButtons_.push_back({ type, handle, rect });
}

void PauseScene::UpdateSelect(void)
{
	// キーボード・パッド
	SelectByAction();

	// マウス
	SelectByMouse();
}

void PauseScene::SelectByAction(void)
{
	if (menuButtons_.empty()) return;

	int size = static_cast<int>(menuButtons_.size());

	// 下へ
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN) && selectIndex_ < size - 1)
	{
		selectIndex_++;
	}

	// 上へ
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP) && selectIndex_ > 0)
	{
		selectIndex_--;
	}
}

void PauseScene::SelectByMouse(void)
{
	// マウスを動かしていないときは、パッドの選択を上書きしない
	if (!IsMouseMoved()) return;

	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	int size = static_cast<int>(menuButtons_.size());
	int index = 0;

	// カーソルが乗っているボタンを先頭から探す
	while (index < size && !Collision2D::HitPointRect(mouseX, mouseY, menuButtons_[index].rect))
	{
		index++;
	}

	// 最後まで見つからなかった
	if (index >= size) return;

	selectIndex_ = index;
}

void PauseScene::Decide(void)
{
	if (menuButtons_.empty()) return;

	if (!IsDecidedByAction() && !IsDecidedByMouse()) return;

	Execute(menuButtons_[selectIndex_].menuType);
}

bool PauseScene::IsDecidedByAction(void)
{
	return InputManager::GetInstance()->IsActionUp(INPUT_INFO::ACTION::DECIDE);
}

bool PauseScene::IsDecidedByMouse(void)
{
	if (!InputManager::GetInstance()->IsActionUp(INPUT_INFO::ACTION::DECIDE)) return false;

	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	// 選択中のボタンの上でクリックしたときだけ決定
	return Collision2D::HitPointRect(mouseX, mouseY, menuButtons_[selectIndex_].rect);
}

void PauseScene::Execute(MENU_TYPE type)
{
	switch (type)
	{
	case MENU_TYPE::CONTINUE:
		InputManager::GetInstance()->SetMouseLock(true);
		SceneManager::GetInstance()->PopScene();
		break;

	case MENU_TYPE::EXIT:
		SceneManager::GetInstance()->PushScene(std::make_shared<ConfirmScene>());
		break;

	default:
		break;
	}
}

bool PauseScene::IsMouseMoved(void)
{
	auto move = InputManager::GetInstance()->GetMouseMove();

	if (move.x != 0.0f) return true;
	if (move.y != 0.0f) return true;

	return false;
}
