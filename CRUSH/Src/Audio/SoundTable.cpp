#include "SoundTable.h"

// サウンドID から サウンドパスを取得

// システム用
namespace SoundTable_System 
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::SYS_BUTTON_DECIDE, { "Data/Audio/SE/System/SYS_BUTTON_DECIDE.mp3", 100} },
		{ SoundID::SYS_BUTTON_CANCEL, { "Data/Audio/SE/System/SYS_BUTTON_CANCEL.mp3", 100} },
		{ SoundID::SYS_SELECT, { "Data/Audio/SE/System/SYS_SELECT.mp3", 100} },
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