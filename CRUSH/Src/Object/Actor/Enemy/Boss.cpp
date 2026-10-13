#include <cmath>
#include "../../../Application.h"
#include "../../../Common/Math/Math.h"
#include "../../../Common/Math/MatrixUtility.h"
#include "../../../Resource/ResourceManager.h"
#include "../../Common/AnimationController/AnimationController.h"
#include "../../Collider/Line/ColliderLine.h"
#include "../../Collider/Capsule/ColliderCapsule.h"
#include "../Player/Player.h"

#include "Boss.h"

namespace
{
	// 視野の広さ
	constexpr float VIEW_RANGE = 600.0f;

	// 視野角
	constexpr float VIEW_ANGLE = 30.0f;

	// 聴覚の広さ
	constexpr float HEARING_RANGE = 400.0f;

	// 足元の線分の始点
	constexpr VECTOR LINE_START_POS = { 0.0f, 10.0f, 0.0f };

	// 足元の線分の終点
	constexpr VECTOR LINE_END_POS = { 0.0f, -10.0f, 0.0f };

	// 当たり判定のカプセルの開始位置
	constexpr VECTOR START_CAPSULE_POS = { 0.0f, 110.0f, 0.0f };

	// 当たり判定のカプセルの終了位置
	constexpr VECTOR END_CAPSULE_POS = { 0.0f, 30.0f, 0.0f };

	// 当たり判定のカプセルの半径
	constexpr float CAPSULE_RADIUS = 20.0f;

	// ボスの最大HP
	constexpr int MAX_HP = 500;
}

Boss::Boss(Player* player)
	:
	player_(player)
{
}

Boss::~Boss(void)
{
}

void Boss::Update(void)
{
	ActorBase::Update();

	// 索敵
	Search();
}

void Boss::Draw(void)
{
	if (isNoticeView_)
	{
		// 視野範囲内：ディフューズカラーを赤色にする
		MV1SetMaterialDifColor(modelId_, 0, GetColorF(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else if (isNoticeHearing_)
	{
		// 聴覚範囲内：ディフューズカラーを黄色にする
		MV1SetMaterialDifColor(modelId_, 0, GetColorF(1.0f, 1.0f, 0.0f, 1.0f));
	}
	else
	{
		// 視野範囲外：ディフューズカラーを灰色にする
		MV1SetMaterialDifColor(modelId_, 0, GetColorF(0.5f, 0.5f, 0.5f, 1.0f));
	}

	ActorBase::Draw();

	// 視野描画
	DrawViewRange();


#ifdef _DEBUG
	// デバッグ表示
	DrawFormatString(0, 130, 0xffffff, "ボスHP：%d / %d", health_.GetHp(), health_.GetMaxHp());

#endif //_DEBUG
}

void Boss::OnHit(int damage)
{
	health_.Damage(damage);
}

void Boss::InitLoad(void)
{
	ResourceManager::GetInstance()->Load(ResourceManager::SRC::BOSS);
}

void Boss::InitTransform(void)
{
	if (modelId_ == -1)
	{
		modelId_ = ResourceManager::GetInstance()->LoadModelDuplicate(
			ResourceManager::SRC::BOSS);
	}

	// モデルの角度
	angle_ = { 0.0f, Math::Deg2Rad(45.0f), 0.0f };
	localAngle_ = { 0.0f, Math::Deg2Rad(180.0f), 0.0f };

	// 角度から方向に変換する
	moveDir_ = { sinf(angle_.y), 0.0f, cosf(angle_.y) };

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = Matrix::Multiplication(localAngle_, angle_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// モデルの位置設定
	pos_ = { 200.0f, 0.0f, 150.0f };
	MV1SetPosition(modelId_, pos_);
}

void Boss::InitCollider(void)
{
	// 足元の線分(地面判定)
	groundLine_ = new ColliderLine(
		ColliderBase::TAG::ENEMY, &pos_, LINE_START_POS, LINE_END_POS);

	// 体のカプセル(壁判定)
	bodyCapsule_ = new ColliderCapsule(
		ColliderBase::TAG::ENEMY, &pos_, START_CAPSULE_POS, END_CAPSULE_POS, CAPSULE_RADIUS);
}

void Boss::InitAnimation(void)
{
	// モデルアニメーション制御の初期化
	animationController_ = new AnimationController(modelId_);

	// アニメーションの追加
	animationController_->Add(
		static_cast<int>(ANIM_TYPE::IDLE), 0.5f, Application::PATH_MODEL + "Player/Idle.mv1");
	animationController_->Add(
		static_cast<int>(ANIM_TYPE::WALK), 0.5f, Application::PATH_MODEL + "Player/Walk.mv1");

	// 初期アニメーションの再生
	animationController_->Play(static_cast<int>(ANIM_TYPE::WALK));
}

void Boss::InitPost(void)
{
	isNoticeView_ = false;
	isNoticeHearing_ = false;

	// HP を満タンにして、0 になったときの通知先を登録する
	health_.Init(MAX_HP);
	health_.SetOnDead([this]() { OnDead(); });
}

void Boss::DrawViewRange(void)
{
	float viewRad = Math::Deg2Rad(VIEW_ANGLE);

	// 向き角度から方向を取得する
	MATRIX mat = MGetIdent();
	mat = Matrix::GetMatrixRotateXYZ(angle_);

	// 前方方向
	VECTOR forward = VTransform(Math::DIR_F, mat);

	// 右側方向
	MATRIX rightMat = MMult(mat, MGetRotY(Math::Deg2Rad(VIEW_ANGLE)));
	VECTOR right = VTransform(Math::DIR_F, rightMat);

	// 左側方向
	MATRIX leftMat = MMult(mat, MGetRotY(Math::Deg2Rad(-VIEW_ANGLE)));
	VECTOR left = VTransform(Math::DIR_F, leftMat);

	// 自分の位置
	VECTOR pos0 = pos_;

	// 正面の位置
	VECTOR pos1 = VAdd(pos0, VScale(forward, VIEW_RANGE));

	// 正面から反時計回り
	VECTOR pos2 = VAdd(pos0, VScale(left, VIEW_RANGE));

	// 正面から時計回り
	VECTOR pos3 = VAdd(pos0, VScale(right, VIEW_RANGE));

	// 視野
	pos0.y = pos1.y = pos2.y = pos3.y = 10.0f;
	DrawTriangle3D(pos0, pos2, pos1, 0xdd77dd, true);
	DrawTriangle3D(pos0, pos1, pos3, 0xdd77dd, true);
	DrawLine3D(pos0, pos1, 0x000000);
	DrawLine3D(pos0, pos2, 0x000000);
	DrawLine3D(pos0, pos3, 0x000000);

	// 聴覚
	DrawCone3D(
		VAdd(pos_, { 0.0f, 0.0f, 0.0f }),
		VAdd(pos_, { 0.0f, 1.0f, 0.0f }),
		HEARING_RANGE, 10, 0xffff7f, 0xffff7f, true);
}

void Boss::Search(void)
{
	// 検知フラグリセット
	isNoticeView_ = false;
	isNoticeHearing_ = false;

	// プレイヤーの座標を取得
	VECTOR pPos = player_->GetPos();

	// エネミーからプレイヤーまでのベクトル
	VECTOR diff = VSub(pPos, pos_);

	// 視野範囲に入っているか判断(ピタゴラスの定理)
	float distance = std::pow(diff.x, 2.0f) + std::pow(diff.z, 2.0f);
	if (distance <= (std::pow(VIEW_RANGE, 2.0f)))
	{
		// エネミーから見たプレイヤーの角度を求める
		float dot = VDot(VNorm(moveDir_), VNorm(diff));
		float angle = acosf(dot);

		// 視野角をラジアンに変換
		const float viewRad = Math::Deg2Rad(VIEW_ANGLE);

		// 視野各内に入っているか判断
		if (angle <= viewRad)
		{
			isNoticeView_ = true;
		}
	}

	// 聴覚
	if (distance <= (std::pow(HEARING_RANGE, 2.0f)))
	{
		isNoticeHearing_ = true;
	}
}

void Boss::OnDead(void)
{
}