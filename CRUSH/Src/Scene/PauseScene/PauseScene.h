#pragma once
#include <vector>
#include <DxLib.h>
#include "../SceneBase.h"

class PauseScene : public SceneBase
{
public:

	// 選択項目の種類
	enum class MENU_TYPE
	{
		NONE = -1,

		CONTINUE,
		EXIT,
	};

	// 選択項目のデータ
	struct MenuData
	{
		MENU_TYPE menuType_;
		int graphHandle_;
		RECT rect_;
	};

	// コンストラクタ
	PauseScene(void);

	// デストラクタ
	~PauseScene(void) override;

	// 初期化
	void Init(void)	override;

	// 読み込み
	void Load(void)	override;

	// 読み込み後の処理
	void LoadEnd(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	// 現在選択されている項目の番号
	int selectIndex_;

	// 選択項目ボタンの情報を格納
	std::vector<MenuData> menuButtons_;

	// ボタンを追加
	void AddButton(MENU_TYPE type, int handle, const RECT& rect);

	// デバイスごとの選択
	void UpdateSelect(void);

	// 選択の変更（キーボード・パッド）
	void SelectByAction(void);

	// 選択の変更（マウス）
	void SelectByMouse(void);

	// 選択中の項目を実行
	void Decide(void);
	
	// キーボード・パッドで決定したか
	bool IsDecidedByAction(void);

	// マウスで決定したか
	bool IsDecidedByMouse(void);

	// 項目ごとの処理を実行
	void Execute(MENU_TYPE type);

	// マウスが動いたかどうか
	bool IsMouseMoved(void);
};