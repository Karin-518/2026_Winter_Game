#include "EffectTable.h"

namespace EffectTable
{
	const std::unordered_map<EFFECT_ID, EffectInfo> Table =
	{
		{ EFFECT_ID::HIT_SPARK,{ ResourceManager::SRC::EFFECT_HIT_SPARK,   50.0f } },
		{ EFFECT_ID::RUSH_DUST,{ ResourceManager::SRC::EFFECT_RUSH_DUST,   80.0f } },
		{ EFFECT_ID::WINDUP_GLOW,{ ResourceManager::SRC::EFFECT_WINDUP_GLOW, 50.0f } },
	};
}