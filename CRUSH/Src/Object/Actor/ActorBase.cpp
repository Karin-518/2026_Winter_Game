#include "../../Application.h"
#include "../../Common/Math/Math.h"
#include "../../Common/Math/MatrixUtility.h"
#include "../Common/AnimationController/AnimationController.h"
#include "../Collider/Line/ColliderLine.h"
#include "../Collider/Capsule/ColliderCapsule.h"
#include "../Collider/Model/ColliderModel.h"

#include "ActorBase.h"

namespace
{
	// 重力
	constexpr float GRAVITY = 0.8f;
}

ActorBase::ActorBase(void)
	:
	animationController_(nullptr),
	animType_(0),
	modelId_(-1),
	pos_({}),
	angle_({}),
	localAngle_({}),
	scale_({}),
	groundLine_(nullptr),
	bodyCapsule_(nullptr),
	preInputDir_({}),
	moveDir_({}),
	jumpPow_(0.0f)
{ 
}

ActorBase::~ActorBase(void)
{
}

void ActorBase::Init(void)
{
	// Transform初期化
	InitTransform();

	// 当たり判定の作成
	if (groundLine_ == nullptr &&
		bodyCapsule_ == nullptr)
	{
		InitCollider();
	}

	// アニメーションの初期化
	InitAnimation();

	// 初期化後の個別処理
	InitPost();
}

void ActorBase::Load(void)
{
	// リソースロード
	InitLoad();
}

void ActorBase::LoadEnd(void)
{
	// 初期化
	Init();
}

void ActorBase::Update(void)
{
	// プレイヤーの遅延回転処理
	DelayRotate();

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = Matrix::Multiplication(localAngle_, angle_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// 重力(加速度を速度に加算していく)
	jumpPow_ -= GRAVITY;

	// プレイヤーの座標に移動量(速度、ジャンプ力)を加算する
	pos_.y += jumpPow_;

	// ステージとの当たり判定(壁 → 地面の順)
	// モデルに座標を渡す前に行うので、押し戻した位置がそのフレームで反映される
	CollisionWall();
	CollisionGround();

	// モデルに座標を設定する
	MV1SetPosition(modelId_, pos_);

	// アニメーションの更新
	animationController_->Update();
}

void ActorBase::Draw(void)
{
	MV1DrawModel(modelId_);

#ifdef _DEBUG

	// 当たり判定のデバッグ表示
	if (bodyCapsule_ != nullptr)
	{
		bodyCapsule_->Draw();
	}

	if (groundLine_ != nullptr)
	{
		groundLine_->Draw();
	}

#endif // _DEBUG
}

void ActorBase::Release(void)
{
	modelId_ = -1;

	animationController_->Release();
	delete animationController_;
	animationController_ = nullptr;

	ReleaseCollider();
}

void ActorBase::AddGroundModel(const ColliderModel* model)
{
	// nullptr は登録しない
	if (model == nullptr) return;

	groundModels_.push_back(model);
}

void ActorBase::AddWallModel(const ColliderModel* model)
{
	// nullptr は登録しない
	if (model == nullptr) return;

	wallModels_.push_back(model);
}

float ActorBase::GetBodyRadius(void) const
{
	// カプセルがなければ0
	if (bodyCapsule_ == nullptr) return 0.0f;

	return bodyCapsule_->GetRadius();
}

void ActorBase::DelayRotate(void)
{
	// 移動方向から角度に変換する
	float goal = atan2f(moveDir_.x, moveDir_.z);

	// 常に最短経路で補間
	angle_.y = Math::LerpAngle(angle_.y, goal, 0.2f);
}

void ActorBase::AddAnimation(int type, ResourceManager::SRC src, float speed)
{
	int handle = ResourceManager::GetInstance()->Load(src).GetHandleId();
	animationController_->Add(type, speed, handle);
}

void ActorBase::CollisionWall(void)
{
	if (bodyCapsule_ == nullptr) return;

	for (const ColliderModel* stage : wallModels_)
	{
		bodyCapsule_->PushBackFromModel(*stage, pos_);
	}
}

void ActorBase::CollisionGround(void)
{
	if (groundLine_ == nullptr) return;

	bool isGround = false;
	for (const ColliderModel* stage : groundModels_)
	{
		isGround = groundLine_->SnapToGround(*stage, pos_) || isGround;
	}

	// 地面に立っていれば落下を止める
	if (isGround) jumpPow_ = 0.0f;
}

void ActorBase::ReleaseCollider(void)
{
	delete groundLine_;
	groundLine_ = nullptr;

	delete bodyCapsule_;
	bodyCapsule_ = nullptr;

	groundModels_.clear();
	wallModels_.clear();
}