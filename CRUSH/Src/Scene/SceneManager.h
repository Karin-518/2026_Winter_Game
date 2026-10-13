#pragma once
#include <list>
#include <memory>

class SceneBase;
class Loading;

class SceneManager
{
public:

	// シーン管理用
	enum class SCENE_ID
	{
		NONE,
		TITLE,
		GAME
	};

public:
	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new SceneManager(); } };
	static SceneManager* GetInstance(void) { return instance_; };
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:

	// 静的インスタンス
	static SceneManager* instance_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	SceneManager(void);
	// デストラクタも同様
	~SceneManager(void);

	// コピー・ムーブ操作を禁止
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
	SceneManager(SceneManager&&) = delete;
	SceneManager& operator=(SceneManager&&) = delete;

	// 下記をコンパイルエラーさせるため 上記を追加
	// SceneManager copy = *SceneManager::GetInstance();
	// SceneManager copied(*SceneManager::GetInstance());
	// SceneManager moved = std::move(*SceneManager::GetInstance());

public:

	void Init(void);	// 初期化
	void Init3D(void);	// 3Dの初期化
	void Update(void);	// 更新
	void Draw(void);	// 描画
	void Delete(void);	// リソースの破棄

	// 状態遷移
	void ChangeScene(std::shared_ptr<SceneBase> scene);

	// シーンを新しく積む
	void PushScene(std::shared_ptr<SceneBase> scene);

	// 最後に追加したシーンを削除する。
	void PopScene(void);

	// 強制的に特定のシーンに飛ぶ。リセットをかけ特定のシーンのみにする。
	void JumpScene(std::shared_ptr<SceneBase> scene);

	// ゲーム終了
	void GameEnd(void) { isGameEnd_ = true; }

	// ゲーム終了取得
	bool GetGameEnd(void) { return isGameEnd_; }

private:

	// 予約の種類
	enum class REQUEST
	{
		NONE,
		CHANGE,
		PUSH,
		POP,
		JUMP,
	};

	// 各種シーン
	std::list<std::shared_ptr<SceneBase>> scenes_;

	// ロード画面
	Loading* load_;

	// ゲーム終了
	bool isGameEnd_;

	// 予約の種類
	REQUEST request_;

	// 予約されたシーン（POP のときは使わない）
	std::shared_ptr<SceneBase> nextScene_;

	// 予約を受け付ける（すでに予約があれば無視する）
	void Request(REQUEST request, std::shared_ptr<SceneBase> scene);

	// 予約があれば実行する
	void ApplyRequest(void);

	// 実際の切り替え処理
	void ApplyChange(std::shared_ptr<SceneBase> scene);
	void ApplyPush(std::shared_ptr<SceneBase> scene);
	void ApplyPop(void);
	void ApplyJump(std::shared_ptr<SceneBase> scene);

	// 一番上のシーンを非同期で読み込む
	void LoadBackScene(void);
};
