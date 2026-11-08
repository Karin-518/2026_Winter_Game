#pragma once
#include <vector>
#include <DxLib.h>
#include "EffectInfo.h"

class EffectManager
{
public:

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new EffectManager(); } }
	static EffectManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

	// 表にあるエフェクトを読み込む（非同期ロードの外で呼ぶ）
	void Load(void);

	// 再生して、再生中のハンドルを返す
	int Play(EFFECT_ID effectId, const VECTOR& pos);

	// 再生中のエフェクトを動かす
	void SetPos(int playingHandle, const VECTOR& pos);

	// 再生中のエフェクトを止める
	void Stop(int playingHandle);

	// 再生中のエフェクトを全部止める
	void StopAll(void);

	// 更新
	void Update(void);

	// 描画（カメラを設定した後、モデルを描いた後に呼ぶ）
	void Draw(void);

private:

	// 静的インスタンス
	static EffectManager* instance_;

	// 再生中のエフェクトのハンドル
	std::vector<int> playingHandles_;

	// 再生が終わったエフェクトを一覧から外す
	void RemoveFinished(void);

	// 外から生成できないようにする
	EffectManager(void);
	~EffectManager(void);

	// コピー・ムーブ操作を禁止
	EffectManager(const EffectManager&) = delete;
	EffectManager& operator=(const EffectManager&) = delete;
	EffectManager(EffectManager&&) = delete;
	EffectManager& operator=(EffectManager&&) = delete;
};