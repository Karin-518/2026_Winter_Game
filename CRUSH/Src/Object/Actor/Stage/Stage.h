#pragma once
#include <DxLib.h>

class ColliderModel;

class Stage
{
public:

	Stage(void);
	~Stage(void);

	void Init(void);
	void Load(void);
	void LoadEnd(void);
	void Update(void);
	void Draw(void);
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

