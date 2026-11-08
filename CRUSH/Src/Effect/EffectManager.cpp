#include <algorithm>
#include <EffekseerForDXLib.h>
#include "../Resource/ResourceManager.h"
#include "EffectTable.h"

#include "EffectManager.h"

// 静的インスタンス
EffectManager* EffectManager::instance_ = nullptr;

EffectManager::EffectManager(void)
	:
	playingHandles_()
{
}

EffectManager::~EffectManager(void)
{
}

void EffectManager::Load(void)
{
	// 表にあるエフェクトを全部読み込む
	for (const auto& [effectId, info] : EffectTable::Table)
	{
		ResourceManager::GetInstance()->Load(info.src);
	}
}

int EffectManager::Play(EFFECT_ID effectId, const VECTOR& pos)
{
	// 表に載っていない種類なら出さない
	auto it = EffectTable::Table.find(effectId);
	if (it == EffectTable::Table.end()) return -1;

	// この種類の、読み込むリソースと拡大率
	const EffectInfo& info = it->second;

	// 読み込んだエフェクトのハンドル（読み込み済みなら、すぐ返ってくる）
	int effectHandle = ResourceManager::GetInstance()->Load(info.src).GetHandleId();
	if (effectHandle == -1) return -1;

	// 再生する
	int playingHandle = PlayEffekseer3DEffect(effectHandle);
	if (playingHandle == -1) return -1;

	// 位置と大きさ
	SetPosPlayingEffekseer3DEffect(playingHandle, pos.x, pos.y, pos.z);
	SetScalePlayingEffekseer3DEffect(playingHandle, info.scale, info.scale, info.scale);

	// 止められるように覚えておく
	playingHandles_.push_back(playingHandle);

	return playingHandle;
}

void EffectManager::SetPos(int playingHandle, const VECTOR& pos)
{
	// 再生できていないハンドルなら何もしない
	if (playingHandle == -1) return;

	SetPosPlayingEffekseer3DEffect(playingHandle, pos.x, pos.y, pos.z);
}

void EffectManager::Stop(int playingHandle)
{
	// 再生できていないハンドルなら何もしない
	if (playingHandle == -1) return;

	StopEffekseer3DEffect(playingHandle);
}

void EffectManager::StopAll(void)
{
	for (int playingHandle : playingHandles_)
	{
		StopEffekseer3DEffect(playingHandle);
	}

	playingHandles_.clear();
}

void EffectManager::Update(void)
{
	UpdateEffekseer3D();

	// 再生が終わったものを一覧から外す
	RemoveFinished();
}

void EffectManager::Draw(void)
{
	// カメラの設定を Effekseer に伝えてから描く
	Effekseer_Sync3DSetting();
	DrawEffekseer3D();
}

void EffectManager::RemoveFinished(void)
{
	// 再生が終わったか（再生中なら 0、終わっていたら -1）
	auto isFinished = [](int playingHandle)
		{
			return IsEffekseer3DEffectPlaying(playingHandle) == -1;
		};

	playingHandles_.erase(
		std::remove_if(playingHandles_.begin(), playingHandles_.end(), isFinished),
		playingHandles_.end());
}