#pragma once

#include <DxLib.h>
class Transform;

class ColliderBase
{
public:

	// 形状
	enum class SHAPE
	{
		NONE,
		LINE,
		CAPSULE,
		MODEL,
	};

	// 衝突種別
	enum class TAG
	{
		STAGE,
		PLAYER,
		ENEMY,
		CAMERA,
	};

	// コンストラクタ
	ColliderBase(SHAPE shape, TAG tag, const VECTOR* followPos);

	// デストラクタ
	virtual ~ColliderBase(void);

	// 描画
	virtual void Draw(void) const;

	// 衝突種別
	TAG GetTag(void) const { return tag_; }

protected:

	// デバッグ表示の色
	static constexpr unsigned int DEBUG_COLOR = 0xff0000;

	// ローカル座標をワールド座標に変換
	VECTOR GetRotPos(const VECTOR& localPos) const;

private:
	
	// 形状
	SHAPE shape_;

	// 衝突種別
	TAG tag_;

	// 追従先
	const VECTOR* followPos_;

};