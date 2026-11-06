#include "SoundTable.h"

// サウンドID から サウンドパスを取得

// システム用
namespace SoundTable_System 
{
	const std::unordered_map<SoundID, SoundData> Table =
	{
		{ SoundID::TITLE_PUSH_BUTTON, { "Data/Audio/SE/System/TITLE_PUSH_BUTTON.mp3", 100} },
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
		// ゲームシーンBGM
		{ SoundID::BGM_BATTLE_1, { "Data/Audio/BGM/Game/BGM_BATTLE_1.mp3", 100} },

		// プレイヤー音
		{ SoundID::SE_PLAYER_MOVE, { "Data/Audio/SE/Player/SE_PLAYER_MOVE.mp3", 150} },
		{ SoundID::SE_PLAYER_SWING_1, { "Data/Audio/SE/Player/SE_PLAYER_SWING_1.mp3", 150} },
		{ SoundID::SE_PLAYER_SWING_2, { "Data/Audio/SE/Player/SE_PLAYER_SWING_2.mp3", 150} },
		{ SoundID::SE_PLAYER_DAMAGE, { "Data/Audio/SE/Player/SE_PLAYER_DAMAGE.mp3", 150} },
		{ SoundID::SE_PLAYER_DODGE, { "Data/Audio/SE/Player/SE_PLAYER_DODGE.mp3", 100} },
		{ SoundID::SE_PLAYER_LOCK_ON, { "Data/Audio/SE/Player/SE_PLAYER_LOCK_ON.mp3", 150} },
		{ SoundID::SE_PLAYER_CAMERA_RESET, { "Data/Audio/SE/Player/SE_PLAYER_CAMERA_RESET.mp3", 100} },

		// 敵音
		{ SoundID::SE_BOSS_MOVE, { "Data/Audio/SE/Boss/SE_BOSS_MOVE.mp3", 150} },
		{ SoundID::SE_BOSS_BITE_SIGN, { "Data/Audio/SE/Boss/SE_BOSS_BITE_SIGN.mp3", 150} },
		{ SoundID::SE_BOSS_BITE, { "Data/Audio/SE/Boss/SE_BOSS_BITE.mp3", 150} },
		{ SoundID::SE_BOSS_RUSH_SIGN, { "Data/Audio/SE/Boss/SE_BOSS_RUSH_SIGN.mp3", 150} },
		{ SoundID::SE_BOSS_RUSH, { "Data/Audio/SE/Boss/SE_BOSS_RUSH.mp3", 150} },
		{ SoundID::SE_BOSS_TAILSPIN_SIGN, { "Data/Audio/SE/Boss/SE_BOSS_TAILSPIN_SIGN.mp3", 150} },
		{ SoundID::SE_BOSS_TAILSPIN, { "Data/Audio/SE/Boss/SE_BOSS_TAILSPIN.mp3", 150} },
		{ SoundID::SE_BOSS_DAMAGE, { "Data/Audio/SE/Boss/SE_BOSS_DAMAGE.mp3", 150} },
		{ SoundID::SE_BOSS_DEAD, { "Data/Audio/SE/Boss/SE_BOSS_DEAD.mp3", 150} },

	};
}