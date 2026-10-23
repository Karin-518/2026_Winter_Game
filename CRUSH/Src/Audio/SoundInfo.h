#pragma once
#include <string>

// サウンドID
enum class SoundID
{
	NON = -1,

	// システム系
	SYS_BUTTON_DECIDE,
	SYS_BUTTON_CANCEL,
	SYS_SELECT,
	SYS_PAUSE,
	SYS_INPACT,
	SYS_CLEAR,
	SYS_GAMEOVER,

	// BGM系
	BGM_TITLE,
	BGM_BATTLE_1,
	BGM_BATTLE_2,
	BGM_GAMEOVER,
	BGM_GAMECLEAR,

	// プレイヤー系
	SE_PLAYER_MOVE,
	SE_PLAYER_SWING_1,
	SE_PLAYER_SWING_2,
	SE_PLAYER_ATTACK_HIT,
	SE_PLAYER_ATTACK_HIT_HEAVY,
	SE_PLAYER_DAMAGE,
	SE_PLAYER_DEAD,
	SE_PLAYER_DODGE,
	SE_PLAYER_SPRINT,
	SE_PLAYER_LOCK_ON,
	SE_PLAYER_CAMERA_RESET,

	// 敵系
	SE_BOSS_ROAR,
	SE_BOSS_MOVE,
	SE_BOSS_BITE_SIGN,
	SE_BOSS_BITE,
	SE_BOSS_RUSH_SIGN,
	SE_BOSS_RUSH,
	SE_BOSS_BREATH_SIGN,
	SE_BOSS_BREATH,
	SE_BOSS_DAMAGE,
	SE_BOSS_DEAD,
};

// 読み込むシーン
enum class LoadScene
{
	SYSTEM,		// システム
	TITLE,		// タイトル
	GAME,		// ゲーム
};

// サウンド属性
struct SoundData
{
	std::string path;	// サウンドのパス
	int volume;			// 音量（0～255）
};

// サウンドの情報
struct SoundResource
{
	int handle;
	int volume;
};
