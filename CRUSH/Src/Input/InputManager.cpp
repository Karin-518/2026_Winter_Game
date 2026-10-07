#include <DxLib.h>
#include "../Application.h"
#include "InputManager.h"

InputManager* InputManager::instance_ = nullptr;

InputManager::~InputManager(void)
{
}

void InputManager::Init(void)
{
	// --- 全キー登録 ---
	for (int k = 0; k < 256; ++k)
	{
		keyStates_[k] = KeyState{};
	}

	// --- アクション初期割り当て ---
	// --- キーボード ---
	// プレイヤー
	SetActionKey(INPUT_INFO::ACTION::MOVE_UP,		{ KEY_INPUT_W });
	SetActionKey(INPUT_INFO::ACTION::MOVE_DOWN,		{ KEY_INPUT_S });
	SetActionKey(INPUT_INFO::ACTION::MOVE_LEFT,		{ KEY_INPUT_A });
	SetActionKey(INPUT_INFO::ACTION::MOVE_RIGHT,	{ KEY_INPUT_D });
	SetActionKey(INPUT_INFO::ACTION::ATTACK,		{ KEY_INPUT_J });
	SetActionKey(INPUT_INFO::ACTION::AVOID,			{ KEY_INPUT_SPACE });
	SetActionKey(INPUT_INFO::ACTION::SPRINT,		{ KEY_INPUT_LSHIFT });
	SetActionKey(INPUT_INFO::ACTION::ROCK,			{ KEY_INPUT_Q });
	SetActionKey(INPUT_INFO::ACTION::CAMERA_RESET,	{ KEY_INPUT_R });
	SetActionKey(INPUT_INFO::ACTION::ITEM,			{ KEY_INPUT_E });
	SetActionKey(INPUT_INFO::ACTION::CAMERA_UP, { KEY_INPUT_UP });
	SetActionKey(INPUT_INFO::ACTION::CAMERA_DOWN, { KEY_INPUT_DOWN });
	SetActionKey(INPUT_INFO::ACTION::CAMERA_LEFT, { KEY_INPUT_LEFT });
	SetActionKey(INPUT_INFO::ACTION::CAMERA_RIGHT, { KEY_INPUT_RIGHT});

	// UI
	SetActionKey(INPUT_INFO::ACTION::UI_MOVE_UP, { KEY_INPUT_W });
	SetActionKey(INPUT_INFO::ACTION::UI_MOVE_DOWN, { KEY_INPUT_S });
	SetActionKey(INPUT_INFO::ACTION::UI_MOVE_LEFT, { KEY_INPUT_A });
	SetActionKey(INPUT_INFO::ACTION::UI_MOVE_RIGHT, { KEY_INPUT_D });
	SetActionKey(INPUT_INFO::ACTION::DECIDE, { KEY_INPUT_SPACE });
	SetActionKey(INPUT_INFO::ACTION::CANCEL, { KEY_INPUT_BACK });

	// システム
	SetActionKey(INPUT_INFO::ACTION::PAUSE, { KEY_INPUT_ESCAPE });

	// --- マウス ---
	// プレイヤー
	SetActionMouse(INPUT_INFO::ACTION::ATTACK, INPUT_INFO::MouseBtn::LEFT);
	SetActionMouse(INPUT_INFO::ACTION::ROCK, INPUT_INFO::MouseBtn::MIDDLE);

	// UI
	SetActionMouse(INPUT_INFO::ACTION::DECIDE, INPUT_INFO::MouseBtn::LEFT);
	SetActionMouse(INPUT_INFO::ACTION::CANCEL, INPUT_INFO::MouseBtn::RIGHT);

	// --- パッド ---
	// プレイヤー
	BindInput stickInput;
	stickInput.type = BindType::PAD_STICK;
	stickInput.pad = INPUT_INFO::JOYPAD_NO::PAD1;
	stickInput.code = (int)INPUT_INFO::PAD_STICK::LEFT_UP;
	AddBind(INPUT_INFO::ACTION::MOVE_UP, 0, stickInput);
	stickInput.code = (int)INPUT_INFO::PAD_STICK::LEFT_DOWN;
	AddBind(INPUT_INFO::ACTION::MOVE_DOWN, 0, stickInput);
	stickInput.code = (int)INPUT_INFO::PAD_STICK::LEFT_LEFT;
	AddBind(INPUT_INFO::ACTION::MOVE_LEFT, 0, stickInput);
	stickInput.code = (int)INPUT_INFO::PAD_STICK::LEFT_RIGHT;
	AddBind(INPUT_INFO::ACTION::MOVE_RIGHT, 0, stickInput);
	SetActionPadBtn(INPUT_INFO::ACTION::ATTACK, INPUT_INFO::JOYPAD_NO::PAD1, INPUT_INFO::PAD_BTN::X);
	SetActionPadBtn(INPUT_INFO::ACTION::AVOID, INPUT_INFO::JOYPAD_NO::PAD1, INPUT_INFO::PAD_BTN::A);
	SetActionPadTrigger(INPUT_INFO::ACTION::SPRINT, INPUT_INFO::JOYPAD_NO::PAD1, INPUT_INFO::PAD_TRIGGER::LT);
	SetActionPadBtn(INPUT_INFO::ACTION::ROCK, INPUT_INFO::JOYPAD_NO::PAD1, INPUT_INFO::PAD_BTN::R3);
	SetActionPadBtn(INPUT_INFO::ACTION::CAMERA_RESET, INPUT_INFO::JOYPAD_NO::PAD1, INPUT_INFO::PAD_BTN::LB);
	SetActionPadBtn(INPUT_INFO::ACTION::ITEM, INPUT_INFO::JOYPAD_NO::PAD1, INPUT_INFO::PAD_BTN::Y);

	stickInput.code = (int)INPUT_INFO::PAD_STICK::RIGHT_UP;
	AddBind(INPUT_INFO::ACTION::CAMERA_UP, 0, stickInput);
	stickInput.code = (int)INPUT_INFO::PAD_STICK::RIGHT_DOWN;
	AddBind(INPUT_INFO::ACTION::CAMERA_DOWN, 0, stickInput);
	stickInput.code = (int)INPUT_INFO::PAD_STICK::RIGHT_LEFT;
	AddBind(INPUT_INFO::ACTION::CAMERA_LEFT, 0, stickInput);
	stickInput.code = (int)INPUT_INFO::PAD_STICK::RIGHT_RIGHT;
	AddBind(INPUT_INFO::ACTION::CAMERA_RIGHT, 0, stickInput);

	// --- UI・システム系 ---
	SetActionPadDir(INPUT_INFO::ACTION::UI_MOVE_UP, INPUT_INFO::JOYPAD_NO::PAD1, INPUT_INFO::PAD_DIR::UP);
	SetActionPadDir(INPUT_INFO::ACTION::UI_MOVE_DOWN, INPUT_INFO::JOYPAD_NO::PAD1, INPUT_INFO::PAD_DIR::DOWN);
	SetActionPadDir(INPUT_INFO::ACTION::UI_MOVE_LEFT, INPUT_INFO::JOYPAD_NO::PAD1, INPUT_INFO::PAD_DIR::LEFT);
	SetActionPadDir(INPUT_INFO::ACTION::UI_MOVE_RIGHT, INPUT_INFO::JOYPAD_NO::PAD1, INPUT_INFO::PAD_DIR::RIGHT);
	stickInput.code = (int)INPUT_INFO::PAD_STICK::LEFT_UP;
	AddBind(INPUT_INFO::ACTION::UI_MOVE_UP, 1, stickInput);
	stickInput.code = (int)INPUT_INFO::PAD_STICK::LEFT_DOWN;
	AddBind(INPUT_INFO::ACTION::UI_MOVE_DOWN, 1, stickInput);
	stickInput.code = (int)INPUT_INFO::PAD_STICK::LEFT_LEFT;
	AddBind(INPUT_INFO::ACTION::UI_MOVE_LEFT, 1, stickInput);
	stickInput.code = (int)INPUT_INFO::PAD_STICK::LEFT_RIGHT;
	AddBind(INPUT_INFO::ACTION::UI_MOVE_RIGHT, 1, stickInput);
	SetActionPadBtn(INPUT_INFO::ACTION::DECIDE, INPUT_INFO::JOYPAD_NO::PAD1, INPUT_INFO::PAD_BTN::A);
	SetActionPadBtn(INPUT_INFO::ACTION::CANCEL, INPUT_INFO::JOYPAD_NO::PAD1, INPUT_INFO::PAD_BTN::B);

	// システム
	SetActionPadBtn(INPUT_INFO::ACTION::PAUSE, INPUT_INFO::JOYPAD_NO::PAD1, INPUT_INFO::PAD_BTN::START);
}

void InputManager::Update(void)
{
	UpdateKeyboard();
	UpdateMouse();

	UpdatePad(INPUT_INFO::JOYPAD_NO::PAD1);
	UpdatePad(INPUT_INFO::JOYPAD_NO::PAD2);
	UpdatePad(INPUT_INFO::JOYPAD_NO::PAD3);
	UpdatePad(INPUT_INFO::JOYPAD_NO::PAD4);
}

void InputManager::UpdateKeyboard(void)
{
	char keyBuf[256];
	GetHitKeyStateAll(keyBuf);

	for (int i = 0; i < 256; ++i)
	{
		auto& st = keyStates_[i];

		st.old = st.now;
		st.now = keyBuf[i] != 0;
		st.down = st.now && !st.old;
		st.up = !st.now && st.old;
	}

	for (int i = 0; i < 256; ++i)
	{
		if (keyStates_[i].down)
		{
			activeDevice_ = ActiveDevice::KEY_MOUSE;
			break;
		}
	}
}

void InputManager::UpdateMouse(void)
{
	int x, y;
	GetMousePoint(&x, &y);

	if (isMouseLock_)
	{
		// マウスの画面固定モード
		// 画面中央からのずれが移動量
		int screenWidth, screenHeight, colorBit;
		GetScreenState(&screenWidth, &screenHeight, &colorBit);
		const int centerX = screenWidth / 2;
		const int centerY = screenHeight / 2;

		mouseMove_ = 
		{
			static_cast<float>(x - centerX), 
			static_cast<float>(y - centerY) 
		};	

		SetMousePoint(centerX, centerY);

		mousePos_ =
		{
			static_cast<float>(centerX),
			static_cast<float>(centerY)
		};
	}
	else
	{
		// マウスの通常モード
		// 前フレームとの差が移動量
		mouseMove_ =
		{
			static_cast<float>(x) - mousePos_.x,
			static_cast<float>(y) - mousePos_.y
		};

		mousePos_ =
		{
			static_cast<float>(x),
			static_cast<float>(y)
		};
	}

	if (mouseMove_.x != 0.0f || mouseMove_.y != 0.0f)
	{
		activeDevice_ = ActiveDevice::KEY_MOUSE;
	}

	int input = GetMouseInput();

	auto updateBtn = [&](INPUT_INFO::MouseBtn btn, int mask)
		{
			auto& st = mouseStates_[(int)btn];
			st.old = st.now;
			st.now = (input & mask) != 0;
			st.down = st.now && !st.old;
			st.up = !st.now && st.old;
		};

	updateBtn(INPUT_INFO::MouseBtn::LEFT, MOUSE_INPUT_LEFT);
	updateBtn(INPUT_INFO::MouseBtn::RIGHT, MOUSE_INPUT_RIGHT);
	updateBtn(INPUT_INFO::MouseBtn::MIDDLE, MOUSE_INPUT_MIDDLE);

	// --- ホイール ---
	int wheel = GetMouseWheelRotVol();

	UpdateKeyState(
		wheel > 0,
		mouseStates_[(int)INPUT_INFO::MouseBtn::WHEEL_UP]);

	UpdateKeyState(
		wheel < 0,
		mouseStates_[(int)INPUT_INFO::MouseBtn::WHEEL_DOWN]);

	// アクティブデバイス更新
	for (int i = 0; i < (int)INPUT_INFO::MouseBtn::MAX; ++i)
	{
		if (mouseStates_[i].down)
		{
			activeDevice_ = ActiveDevice::KEY_MOUSE;
			break;
		}
	}
}

void InputManager::UpdatePad(INPUT_INFO::JOYPAD_NO pad)
{
	auto& st = padStates_[(int)pad];

	XINPUT_STATE xs{};
	ZeroMemory(&xs, sizeof(xs));

	if (GetJoypadXInputState(ToDxPad(pad), &xs) != 0)
	{
		return;
	}

	st.lx = (int)xs.ThumbLX;
	st.ly = (int)xs.ThumbLY;
	st.rx = (int)xs.ThumbRX;
	st.ry = (int)xs.ThumbRY;

	// --- 十字キー ---
	for (int i = 0; i < (int)INPUT_INFO::PAD_DIR::MAX; ++i)
	{
		bool pressed = xs.Buttons[INPUT_INFO::PAD_DIR_MAP[i]] != 0;
		UpdateKeyState(pressed, st.dir[i]);
	}

	// --- ボタン ---
	for (int i = 0; i < (int)INPUT_INFO::PAD_BTN::MAX; ++i)
	{
		bool pressed = xs.Buttons[INPUT_INFO::PAD_BTN_MAP[i]] != 0;
		UpdateKeyState(pressed, st.btn[i]);
	}

	// --- トリガー ---
	st.ltValue = xs.LeftTrigger;
	st.rtValue = xs.RightTrigger;

	UpdateKeyState(st.ltValue > 30, st.lt);
	UpdateKeyState(st.rtValue > 30, st.rt);

	// パッドスティック
	float lx = st.lx / ANALOG_MAX;
	float ly = st.ly / ANALOG_MAX;

	float rx = st.rx / ANALOG_MAX;
	float ry = st.ry / ANALOG_MAX;

	const float threshold = 0.4f;

	UpdateKeyState(ly > threshold, st.stick[(int)INPUT_INFO::PAD_STICK::LEFT_UP]);
	UpdateKeyState(ly < -threshold, st.stick[(int)INPUT_INFO::PAD_STICK::LEFT_DOWN]);
	UpdateKeyState(lx < -threshold, st.stick[(int)INPUT_INFO::PAD_STICK::LEFT_LEFT]);
	UpdateKeyState(lx > threshold, st.stick[(int)INPUT_INFO::PAD_STICK::LEFT_RIGHT]);

	UpdateKeyState(ry > threshold, st.stick[(int)INPUT_INFO::PAD_STICK::RIGHT_UP]);
	UpdateKeyState(ry < -threshold, st.stick[(int)INPUT_INFO::PAD_STICK::RIGHT_DOWN]);
	UpdateKeyState(rx < -threshold, st.stick[(int)INPUT_INFO::PAD_STICK::RIGHT_LEFT]);
	UpdateKeyState(rx > threshold, st.stick[(int)INPUT_INFO::PAD_STICK::RIGHT_RIGHT]);

	// ボタン
	for (int i = 0; i < (int)INPUT_INFO::PAD_BTN::MAX; ++i)
	{
		if (st.btn[i].down)
		{
			activeDevice_ = ActiveDevice::PAD;
			return;
		}
	}

	// 十字キー
	for (int i = 0; i < (int)INPUT_INFO::PAD_DIR::MAX; ++i)
	{
		if (st.dir[i].down)
		{
			activeDevice_ = ActiveDevice::PAD;
			return;
		}
	}

	// トリガー
	if (st.lt.down || st.rt.down)
	{
		activeDevice_ = ActiveDevice::PAD;
		return;
	}

	if (fabs(lx) > 0.4f || fabs(ly) > 0.4f)
	{
		activeDevice_ = ActiveDevice::PAD;
	}

	if (sqrtf(rx * rx + ry * ry) > DEAD_ZONE)
	{
		activeDevice_ = ActiveDevice::PAD;
	}
}

void InputManager::UpdateKeyState(bool isPressed, KeyState& st)
{
	st.old = st.now;
	st.now = isPressed;
	st.down = st.now && !st.old;
	st.up = !st.now && st.old;
}

bool InputManager::IsInputAlreadyUsed(const BindInput& input, INPUT_INFO::ACTION ignoreAction) const
{
	for (const auto& [action, bind] : actionBinds_)
	{
		if (action == ignoreAction)
			continue;

		const auto& arr =
			(input.type == BindType::KEY ||
				input.type == BindType::MOUSE)
			? bind.keyMouse
			: bind.pad;

		for (const auto& b : arr)
		{
			if (b.code == input.code &&
				b.type == input.type &&
				b.pad == input.pad)
			{
				return true;
			}
		}
	}

	return false;
}

void InputManager::RemoveDuplicateOtherActions(INPUT_INFO::ACTION action, const BindInput& input)
{
	auto myCategory = INPUT_INFO::GetActionCategory(action);

	for (auto& [act, bind] : actionBinds_)
	{
		if (act == action)
			continue;

		if (INPUT_INFO::GetActionCategory(act) != myCategory)
			continue;

		auto& arr =
			(input.type == BindType::KEY ||
				input.type == BindType::MOUSE)
			? bind.keyMouse
			: bind.pad;

		for (auto& b : arr)
		{
			if (b.code == input.code &&
				b.type == input.type &&
				b.pad == input.pad)
			{
				b.code = -1;
			}
		}
	}
}

bool InputManager::IsAction(INPUT_INFO::ACTION action) const
{
	const auto& bind = actionBinds_.at(action);

	// キーボード&マウス
	for (const auto& b : bind.keyMouse)
	{
		if (b.code == -1) continue;

		if (b.type == BindType::KEY &&
			keyStates_.at(b.code).now)
			return true;
		
		if (b.type == BindType::MOUSE &&
			mouseStates_[b.code].now)
			return true;
	}

	// パッド
	for (const auto& b : bind.pad)
	{
		if (b.code == -1) continue;

		auto pad = b.pad;

		switch (b.type)
		{
		case BindType::PAD_BTN:
			if (padStates_[(int)pad].btn[b.code].now)
				return true;
			break;

		case BindType::PAD_DIR:
			if (padStates_[(int)pad].dir[b.code].now)
				return true;
			break;

		case BindType::PAD_TRIGGER :
		{
			bool pressed = false;

			if (b.code == (int)INPUT_INFO::PAD_TRIGGER::LT)
				pressed = padStates_[(int)pad].lt.now;
			else
				pressed = padStates_[(int)pad].rt.now;

			if (pressed)
				return true;
		}
			break;

		case BindType::PAD_STICK:
		{
			if (padStates_[(int)pad].stick[b.code].now)
				return true;
		}
			break;

		default:
			break;
		}
	}

	return false;
}

bool InputManager::IsActionDown(INPUT_INFO::ACTION action) const
{
	const auto& bind = actionBinds_.at(action);

	// キーボード&マウス
	for (const auto& b : bind.keyMouse)
	{
		if (b.code == -1) continue;

		if (b.type == BindType::KEY &&
			keyStates_.at(b.code).down)
			return true;

		if (b.type == BindType::MOUSE &&
			mouseStates_[b.code].down)
			return true;
	}

	// パッド
	for (const auto& b : bind.pad)
	{
		if (b.code == -1) continue;

		auto pad = b.pad;

		switch (b.type)
		{
		case BindType::PAD_BTN:
			if (padStates_[(int)pad].btn[b.code].down)
				return true;
			break;

		case BindType::PAD_DIR:
			if (padStates_[(int)pad].dir[b.code].down)
				return true;
			break;

		case BindType::PAD_TRIGGER:
		{
			bool pressed = false;

			if (b.code == (int)INPUT_INFO::PAD_TRIGGER::LT)
				pressed = padStates_[(int)pad].lt.down;
			else
				pressed = padStates_[(int)pad].rt.down;

			if (pressed)
				return true;
		}
			break;

		case BindType::PAD_STICK:
		{
			if (padStates_[(int)pad].stick[b.code].down)
				return true;
		}
		break;

		default:
			break;
		}
	}

	return false;
}

bool InputManager::IsActionUp(INPUT_INFO::ACTION action) const
{
	const auto& bind = actionBinds_.at(action);

	// キーボード&マウス
	for (const auto& b : bind.keyMouse)
	{
		if (b.code == -1) continue;

		if (b.type == BindType::KEY &&
			keyStates_.at(b.code).up)
			return true;

		if (b.type == BindType::MOUSE &&
			mouseStates_[b.code].up)
			return true;
	}

	// パッド
	for (const auto& b : bind.pad)
	{
		if (b.code == -1) continue;

		auto pad = b.pad;

		switch (b.type)
		{
		case BindType::PAD_BTN:
			if (padStates_[(int)pad].btn[b.code].up)
				return true;
			break;

		case BindType::PAD_DIR:
			if (padStates_[(int)pad].dir[b.code].up)
				return true;
			break;

		case BindType::PAD_TRIGGER:
		{
			bool pressed = false;

			if (b.code == (int)INPUT_INFO::PAD_TRIGGER::LT)
				pressed = padStates_[(int)pad].lt.up;
			else
				pressed = padStates_[(int)pad].rt.up;

			if (pressed)
				return true;
		}
			break;

		case BindType::PAD_STICK:
		{
			if (padStates_[(int)pad].stick[b.code].up)
				return true;
		}
		break;

		default:
			break;
		}
	}

	return false;
}

void InputManager::SetActionKey(INPUT_INFO::ACTION action, const std::vector<int>& keys)
{
	auto& bind = actionBinds_[action];

	for (int i = 0; i < MAX_BIND_PER_TYPE; ++i)
	{
		bind.keyMouse[i].code = -1;
	}

	for (int i = 0;
		i < (int)keys.size() && i < MAX_BIND_PER_TYPE;
		++i)
	{
		bind.keyMouse[i].type = BindType::KEY;
		bind.keyMouse[i].code = keys[i];
	}
}

void InputManager::SetActionMouse(INPUT_INFO::ACTION action, INPUT_INFO::MouseBtn btn)
{
	auto& bind = actionBinds_[action];

	for (int i = 0; i < MAX_BIND_PER_TYPE; ++i)
	{
		if (bind.keyMouse[i].code == -1)
		{
			bind.keyMouse[i].type = BindType::MOUSE;
			bind.keyMouse[i].code = (int)btn;
			return;
		}
	}
}

void InputManager::SetActionPadBtn(INPUT_INFO::ACTION action, INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_BTN btn)
{
	auto& bind = actionBinds_[action];

	for (int i = 0; i < MAX_BIND_PER_TYPE; ++i)
	{
		if (bind.pad[i].code == -1)
		{
			bind.pad[i].type = BindType::PAD_BTN;
			bind.pad[i].pad = pad;
			bind.pad[i].code = (int)btn;
			return;
		}
	}
}

void InputManager::SetActionPadDir(INPUT_INFO::ACTION action, INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_DIR dir)
{
	auto& bind = actionBinds_[action];

	for (int i = 0; i < MAX_BIND_PER_TYPE; ++i)
	{
		if (bind.pad[i].code == -1)
		{
			bind.pad[i].type = BindType::PAD_DIR;
			bind.pad[i].pad = pad;
			bind.pad[i].code = (int)dir;
			return;
		}
	}
}

void InputManager::SetActionPadTrigger(INPUT_INFO::ACTION action, INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_TRIGGER trigger)
{
	auto& bind = actionBinds_[action];

	for (int i = 0; i < MAX_BIND_PER_TYPE; ++i)
	{
		if (bind.pad[i].code == -1)
		{
			bind.pad[i].type = BindType::PAD_TRIGGER;
			bind.pad[i].pad = pad;
			bind.pad[i].code = (int)trigger;
			return;
		}
	}
}

Vector2 InputManager::GetMousePos(void) const
{
	return mousePos_;
}

Vector2 InputManager::GetMouseMove(void) const
{
	return mouseMove_;
}

void InputManager::SetMouseLock(bool isLock)
{
	if (isMouseLock_ == isLock) return;

	isMouseLock_ = isLock;

	if (isMouseLock_)
	{
		// カーソルを非表示にして画面中央へ設定
		int screenWidth, screenHeight, colorBit;

		GetScreenState(&screenWidth, &screenHeight, &colorBit);

		SetMousePoint(screenWidth / 2, screenHeight / 2);

		mousePos_ = 
		{ 
			static_cast<float>(screenWidth / 2), 
			static_cast<float>(screenHeight / 2) 
		};

		SetMouseDispFlag(FALSE);
	}
	else
	{
		SetMouseDispFlag(TRUE);
	}

	mouseMove_ = { 0.0f, 0.0f };
}

bool InputManager::IsMouseLock(void) const
{
	return isMouseLock_;
}

bool InputManager::IsMouse(INPUT_INFO::MouseBtn btn) const
{
	return mouseStates_[(int)btn].now;
}

bool InputManager::IsMouseDown(INPUT_INFO::MouseBtn btn) const
{
	return mouseStates_[(int)btn].down;
}

bool InputManager::IsMouseUp(INPUT_INFO::MouseBtn btn) const
{
	return mouseStates_[(int)btn].up;
}

bool InputManager::IsPadDir(INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_DIR dir) const
{
	return padStates_[(int)pad].dir[(int)dir].now;
}

bool InputManager::IsPadDirDown(INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_DIR dir) const
{
	return padStates_[(int)pad].dir[(int)dir].down;
}

bool InputManager::IsPadDirUp(INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_DIR dir) const
{
	return padStates_[(int)pad].dir[(int)dir].up;
}

bool InputManager::IsPadLT(INPUT_INFO::JOYPAD_NO pad) const
{
	return padStates_[(int)pad].lt.now;
}

bool InputManager::IsPadRT(INPUT_INFO::JOYPAD_NO pad) const
{
	return padStates_[(int)pad].rt.now;
}

float InputManager::GetPadLTValue(INPUT_INFO::JOYPAD_NO pad) const
{
	return padStates_[(int)pad].ltValue / 255.0f;
}

float InputManager::GetPadRTValue(INPUT_INFO::JOYPAD_NO pad) const
{
	return padStates_[(int)pad].rtValue / 255.0f;
}

VECTOR InputManager::GetDirectionXZAKey(INPUT_INFO::JOYPAD_NO pad) const
{
	const auto& st = padStates_[(int)pad];

	float x = st.lx / ANALOG_MAX;
	float z = st.ly / ANALOG_MAX;

	float len = sqrtf(x * x + z * z);
	if (len < DEAD_ZONE) return VGet(0, 0, 0);

	float scale = (len - DEAD_ZONE) / (1.0f - DEAD_ZONE);
	x = (x / len) * scale;
	z = (z / len) * scale;

	return VNorm(VGet(x, 0, -z));
}

VECTOR InputManager::GetRightStickDirection(INPUT_INFO::JOYPAD_NO pad) const
{
	const auto& st = padStates_[(int)pad];

	float x = st.rx / ANALOG_MAX;
	float z = st.ry / ANALOG_MAX;

	float len = sqrtf(x * x + z * z);
	if (len < DEAD_ZONE) return VGet(0, 0, 0);

	float scale = (len - DEAD_ZONE) / (1.0f - DEAD_ZONE);
	x = (x / len) * scale;
	z = (z / len) * scale;

	return VNorm(VGet(x, 0, -z));
}

Vector2 InputManager::GetRightStickAnalog(INPUT_INFO::JOYPAD_NO pad) const
{
	const auto& st = padStates_[(int)pad];

	float x = st.rx / ANALOG_MAX;
	float y = st.ry / ANALOG_MAX;

	float len = sqrtf(x * x + y * y);
	if (len < DEAD_ZONE) return { 0.0f, 0.0f };

	// 傾きの大きさ(1.0を上限)をデッドゾーン分だけ差し引いて 0.0 ~ 1.0 に直す
	float mag = len > 1.0f ? 1.0f : len;
	float scale = (mag - DEAD_ZONE) / (1.0f - DEAD_ZONE);

	// 向きは保ったまま、大きさだけを scale にする(上に倒すとYがマイナス)
	return { (x / len) * scale, -(y / len) * scale };
}

void InputManager::ClearActionBind(INPUT_INFO::ACTION action)
{
	actionBinds_[action] = ActionBind{};
}

void InputManager::ClearBindByType(INPUT_INFO::ACTION action, BindType type, INPUT_INFO::JOYPAD_NO pad)
{
	auto& bind = actionBinds_[action];

	auto& arr =
		(type == BindType::KEY ||
			type == BindType::MOUSE)
		? bind.keyMouse
		: bind.pad;

	for (auto& b : arr)
	{
		if (b.type == type)
		{
			if (type == BindType::PAD_BTN ||
				type == BindType::PAD_DIR ||
				type == BindType::PAD_TRIGGER)
			{
				if (b.pad == pad)
					b.code = -1;
			}
			else
			{
				b.code = -1;
			}
		}
	}
}

void InputManager::ClearKeyMouse(INPUT_INFO::ACTION action)
{
	auto& bind = actionBinds_[action];
	for (auto& b : bind.keyMouse)
		b.code = -1;
}

void InputManager::ClearPad(INPUT_INFO::ACTION action)
{
	auto& bind = actionBinds_[action];
	for (auto& b : bind.pad)
		b.code = -1;
}

int InputManager::CountPadBtn(const ActionBind& bind, INPUT_INFO::JOYPAD_NO pad) const
{
	int count = 0;
	for (const auto& b : bind.pad)
	{
		if (b.type == BindType::PAD_BTN &&
			b.pad == pad &&
			b.code != -1)
		{
			count++;
		}
	}
	return count;
}

int InputManager::CountPadDir(const ActionBind& bind, INPUT_INFO::JOYPAD_NO pad) const
{
	int count = 0;
	for (const auto& b : bind.pad)
	{
		if (b.type == BindType::PAD_DIR &&
			b.pad == pad &&
			b.code != -1)
		{
			count++;
		}
	}
	return count;
}

int InputManager::CountPadTrigger(const ActionBind& bind, INPUT_INFO::JOYPAD_NO pad) const
{
	int count = 0;
	for (const auto& b : bind.pad)
	{
		if (b.type == BindType::PAD_TRIGGER &&
			b.pad == pad &&
			b.code != -1)
		{
			count++;
		}
	}
	return count;
}

void InputManager::AddBind(INPUT_INFO::ACTION action, int slot,	const BindInput& input)
{
	if (slot < 0 || slot >= MAX_BIND_PER_TYPE)
		return;
	
	auto& bind = actionBinds_[action];

	auto& targetArray = (
		input.type == BindType::KEY ||
		input.type == BindType::MOUSE)
		? bind.keyMouse
		: bind.pad;

	// 同じスロットに同じ入力なら削除
	if (targetArray[slot].code == input.code &&
		targetArray[slot].type == input.type &&
		targetArray[slot].pad == input.pad)
	{
		targetArray[slot].code = -1;
		return;
	}

	// 同一アクション内の重複チェック
	for (int i = 0; i < MAX_BIND_PER_TYPE; ++i)
	{
		if(targetArray[i].code == input.code &&
			targetArray[i].type == input.type &&
			targetArray[i].pad == input.pad)
		{
			return;
		}
	}

	// 他アクションからは削除
	RemoveDuplicateOtherActions(action, input);

	targetArray[slot] = input;
}

bool InputManager::DetectKey(int& outkey)
{
	for (int i = 0; i < 256; ++i)
	{
		if (keyStates_[i].down)
		{
			outkey = i;
			return true;
		}
	}
	return false;
}

bool InputManager::DetectMouse(INPUT_INFO::MouseBtn& outBtn)
{
	for (int i = 0; i < (int)INPUT_INFO::MouseBtn::MAX; ++i)
	{
		if (mouseStates_[i].down)
		{
			outBtn = (INPUT_INFO::MouseBtn)i;
			return true;
		}
	}
	return false;
}

bool InputManager::DetectPadBtn(INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_BTN& outBtn)
{
	const auto& st = padStates_[(int)pad];

	for (int i = 0; i < (int)INPUT_INFO::PAD_BTN::MAX; ++i)
	{
		if (st.btn[i].down)
		{
			outBtn = (INPUT_INFO::PAD_BTN)i;
			return true;
		}
	}
	return false;
}

bool InputManager::DetectPadDir(INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_DIR& outdir)
{
	const auto& st = padStates_[(int)pad];

	for (int i = 0; i < (int)INPUT_INFO::PAD_DIR::MAX; ++i)
	{
		if (st.dir[i].down)
		{
			outdir = (INPUT_INFO::PAD_DIR)i;
			return true;
		}
	}
	return false;
}

bool InputManager::DetectPadTrigger(INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_TRIGGER& out)
{
	const auto& st = padStates_[(int)pad];

	if (st.lt.down)
	{
		out = INPUT_INFO::PAD_TRIGGER::LT;
		return true;
	}

	if (st.rt.down)
	{
		out = INPUT_INFO::PAD_TRIGGER::RT;
		return true;
	}

	return false;
}

bool InputManager::DetectPadStick(INPUT_INFO::JOYPAD_NO pad, INPUT_INFO::PAD_STICK& out)
{
	const auto& st = padStates_[(int)pad];

	for (int i = 0; i < (int)INPUT_INFO::PAD_STICK::MAX; ++i)
	{
		if (st.stick[i].down)
		{
			out = (INPUT_INFO::PAD_STICK)i;
			return true;
		}
	}

	return false;
}

void InputManager::SetActionBinds(const std::map<INPUT_INFO::ACTION, ActionBind>& binds)
{
	actionBinds_ = binds;
}
