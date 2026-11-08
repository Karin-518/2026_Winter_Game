#pragma once
#include "../Resource/ResourceManager.h"

// エフェクトの種類
enum class EFFECT_ID
{
	HIT_SPARK,	// ヒット火花
	RUSH_DUST,	// 突進の土煙
	WINDUP_GLOW,// 予備動作の光
};

// エフェクト1種類分の情報
struct EffectInfo
{
	ResourceManager::SRC src;		// 読み込むリソース
	float scale;					// 再生するときの拡大率
};