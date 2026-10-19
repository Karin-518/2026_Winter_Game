#pragma once
#include <string>
#include <vector>
#include <DxLib.h>
#include "../../Resource/ResourceManager.h"

class AnimationController;
class ColliderLine;
class ColliderCapsule;
class ColliderModel;

class ActorBase
{
public:

	ActorBase(void);			// コンストラクタ
	virtual ~ActorBase(void);	// デストラクタ

	void Init(void);			// 初期化
	void Load(void);			// 読み込み
	void LoadEnd(void);			// 読み込み後
	virtual void Update(void);	// 更新
	virtual void Draw(void);	// 描画
	virtual void Release(void);	// 解放

	// 座標を取得
	const VECTOR& GetPos(void) const { return pos_; }

	// 地面判定の相手を登録
	void AddGroundModel(const ColliderModel* model);

	// 壁判定の相手を登録
	void AddWallModel(const ColliderModel* model);

	// 攻撃が当たりそうなときに呼ばれる
	virtual void OnHit(int damage) {}

protected:

	// アニメーション制御
	AnimationController* animationController_;

	// アニメーション種別
	int animType_;

	// モデル情報
	int modelId_;
	VECTOR pos_;
	VECTOR angle_;
	VECTOR localAngle_;
	VECTOR scale_;

	// 足元の線分(地面判定用)
	ColliderLine* groundLine_;

	// 体のカプセル(壁判定用)
	ColliderCapsule* bodyCapsule_;

	// 地面判定の相手
	std::vector<const ColliderModel*> groundModels_;

	// 壁判定の相手
	std::vector<const ColliderModel*> wallModels_;

	// パッド入力値の補間
	VECTOR preInputDir_;

	// 移動方向
	VECTOR moveDir_;

	// ジャンプ力
	float jumpPow_;

	// リソースロード
	virtual void InitLoad(void) = 0;

	// 大きさ、回転、座標の初期化
	virtual void InitTransform(void) = 0;

	// 当たり判定の作成(派生クラスで形と大きさを決める)
	virtual void InitCollider(void) = 0;

	// アニメーションの初期化
	virtual void InitAnimation(void) = 0;

	// 初期化後の個別処理
	virtual void InitPost(void) = 0;

	// 遅延回転処理
	void DelayRotate(void);

	// リソースからアニメーションを登録する
	void AddAnimation(int type, ResourceManager::SRC src, float speed);

	template <typename T>
	void AddAnimation(T type, ResourceManager::SRC src, float speed)
	{
		AddAnimation(static_cast<int>(type), src, speed);
	}

private:

	// 壁から押し出す
	void CollisionWall(void);

	// 地面に合わせる
	void CollisionGround(void);

	// 当たり判定の解放
	void ReleaseCollider(void);

};
