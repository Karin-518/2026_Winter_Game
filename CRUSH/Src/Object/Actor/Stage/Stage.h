#pragma once
#include <DxLib.h>

class ColliderModel;

class Stage
{
public:

	// コンストラクタ
	Stage(void);

	// デストラクタ
	~Stage(void);

	// 初期化
	void Init(void);

	// ロード
	void Load(void);

	// ロード後
	void LoadEnd(void);

	// 更新
	void Update(void);
	
	// 描画
	void Draw(void);
	
	// 解放
	void Release(void);

	// ステージの当たり判定
	const ColliderModel* GetCollider(void) const { return collider_; }

private:

	// ステージモデルのハンドルID
	int modelId_;

	// ステージモデルの位置
	VECTOR pos_;

	// ステージの当たり判定
	ColliderModel* collider_;
};

