#include "../../Application.h"
#include "../../Input/InputManager.h"
#include "../../Common/Collision/Collision.h"
#include "../SceneManager.h"

#include "ConfirmScene.h"

namespace
{
	// はい
	constexpr RECT YES_RECT = { 250, 250, 500, 310 };

	// いいえ
	constexpr RECT NO_RECT = { 250, 340, 500, 400 };

	// 初期選択
	constexpr int DEFAULT_SELECT_INDEX = 1;

	// 無効なハンドル
	constexpr int INVALID_HANDLE = -1;

	// 確認メッセージの位置
	constexpr int MESSAGE_POS_X = 250;
	constexpr int MESSAGE_POS_Y = 180;

	// デバッグ表示の色
	constexpr unsigned int TEXT_COLOR = 0xffffff;
	constexpr unsigned int SELECT_COLOR = 0xffffff;
	constexpr unsigned int UNSELECT_COLOR = 0x808080;

	// デバッグ表示の文字位置
	constexpr int TEXT_OFFSET_X = 20;
	constexpr int TEXT_OFFSET_Y = 15;

	// デバッグ表示用の項目名
	const char* GetMenuLabel(ConfirmScene::MENU_TYPE type)
	{
		switch (type)
		{
		case ConfirmScene::MENU_TYPE::YES:	return "はい";
		case ConfirmScene::MENU_TYPE::NO:	return "いいえ";
		default:								return "";
		}
	}
}

ConfirmScene::ConfirmScene(void)
	:
	selectIndex_(DEFAULT_SELECT_INDEX),
	menuButtons_{}
{
}

ConfirmScene::~ConfirmScene(void)
{
}

void ConfirmScene::Init(void)
{
	selectIndex_ = DEFAULT_SELECT_INDEX;

	// 確認中はマウスカーソルを動かせるようにする
	InputManager::GetInstance()->SetMouseLock(false);
}

void ConfirmScene::Load(void)
{
	// 二重に読み込まれてもボタンが増えないようにする
	menuButtons_.clear();

	// ボタンの追加（DEFAULT_SELECT_INDEX と順番を合わせる）
	AddButton(MENU_TYPE::YES, INVALID_HANDLE, YES_RECT);
	AddButton(MENU_TYPE::NO, INVALID_HANDLE, NO_RECT);
}

void ConfirmScene::LoadEnd(void)
{
	Init();
}

void ConfirmScene::Update(void)
{
	// キャンセルボタンを押したら「いいえ」と同じ扱い
	// ※アクション名は手元の InputManager に合わせてください
	if (InputManager::GetInstance()->IsActionUp(INPUT_INFO::ACTION::CANCEL))
	{
		Execute(MENU_TYPE::NO);
		return;
	}

	// 選択処理
	UpdateSelect();

	// 選択したものを実行
	Decide();
}

void ConfirmScene::Draw(void)
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	DrawString(MESSAGE_POS_X, MESSAGE_POS_Y, "ゲームを終了しますか？", TEXT_COLOR);

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

void ConfirmScene::Release(void)
{
	menuButtons_.clear();
}

void ConfirmScene::AddButton(MENU_TYPE type, int handle, const RECT& rect)
{
	menuButtons_.push_back({ type, handle, rect });
}

void ConfirmScene::UpdateSelect(void)
{
	// キーボード・パッド
	SelectByAction();

	// マウス
	SelectByMouse();
}

void ConfirmScene::SelectByAction(void)
{
	if (menuButtons_.empty()) return;

	int size = static_cast<int>(menuButtons_.size());

	// 下へ
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
	{
		selectIndex_ = (selectIndex_ + 1) % size;
	}

	// 上へ
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP))
	{
		selectIndex_ = (selectIndex_ - 1 + size) % size;
	}
}

void ConfirmScene::SelectByMouse(void)
{
	// マウスを動かしていないときは、パッドの選択を上書きしない
	if (!IsMouseMoved()) return;

	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	int size = static_cast<int>(menuButtons_.size());
	int index = 0;

	// カーソルが乗っているボタンを先頭から探す（見つかったらそこで止まる）
	while (index < size && !Collision2D::HitPointRect(mouseX, mouseY, menuButtons_[index].rect))
	{
		index++;
	}

	// 最後まで見つからなかった
	if (index >= size) return;

	selectIndex_ = index;
}

void ConfirmScene::Decide(void)
{
	if (menuButtons_.empty()) return;

	if (!IsDecidedByAction() && !IsDecidedByMouse()) return;

	Execute(menuButtons_[selectIndex_].menuType);
}

bool ConfirmScene::IsDecidedByAction(void)
{
	return InputManager::GetInstance()->IsActionUp(INPUT_INFO::ACTION::DECIDE);
}

bool ConfirmScene::IsDecidedByMouse(void)
{
	if (!InputManager::GetInstance()->IsActionUp(INPUT_INFO::ACTION::DECIDE)) return false;

	int mouseX, mouseY;
	GetMousePoint(&mouseX, &mouseY);

	// 選択中のボタンの上でクリックしたときだけ決定
	return Collision2D::HitPointRect(mouseX, mouseY, menuButtons_[selectIndex_].rect);
}

void ConfirmScene::Execute(MENU_TYPE type)
{
	switch (type)
	{
	case MENU_TYPE::YES:
		SceneManager::GetInstance()->GameEnd();
		break;

	case MENU_TYPE::NO:
		// 呼び出し元（タイトル・ポーズ）に戻る
		SceneManager::GetInstance()->PopScene();
		break;

	default:
		break;
	}
}

bool ConfirmScene::IsMouseMoved(void)
{
	auto move = InputManager::GetInstance()->GetMouseMove();

	if (move.x != 0.0f) return true;
	if (move.y != 0.0f) return true;

	return false;
}