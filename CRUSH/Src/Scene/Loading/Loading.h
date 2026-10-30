#pragma once

class Loading
{
public:

	// コンストラクタ
	Loading(void);

	// デストラクタ
	~Loading(void);

	// 初期化
	void Init(void);

	// 読み込み
	void Load(void);

	// 更新
	void Update(void);

	// 描画
	void Draw(void);

	// 解放
	void Release(void);
	
	// 非同期ロードの開始
	void StartAsyncLoad(void);
	
	// 非同期ロードの終了
	void EndAsyncLoad(void);

	// ロード中かを返す
	bool IsLoading(void) { return isLoading_; }

private:

	// 最低でもロード画面を表示する時間
	static constexpr int MIN_LOAD_TIME = 60;	// 60fps(1秒) * x

	// 画像ハンドル
	int handle_;

	// X座標
	float posX_;

	// Y座標
	float posY_;

	// ロード中の判定用
	bool isLoading_;

	// 最低でもロード画面を表示する時間の範囲
	int loadTimer_;
};