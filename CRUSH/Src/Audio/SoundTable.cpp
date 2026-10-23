#include "SoundTable.h"

// サウンドID から サウンドパスを取得

// システム用
namespace SoundTable_System 
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
	};
}

// タイトル画面用
namespace SoundTable_Title
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
	};
}

// ゲーム画面用
namespace SoundTable_Game 
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::SE_PLAYER_SWING_1, { "Data/Audio/SE/Player/SE_PLAYER_SWING_1.mp3", 150} },

	};
}
