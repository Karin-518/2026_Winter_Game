#include <DxLib.h>

#include "HpBar.h"

namespace
{
	// ”wŒiiŒ¸‚Á‚½•”•ªj‚ÌF
	constexpr unsigned int BACK_COLOR = 0x303030;

	// ˜g‚ÌF
	constexpr unsigned int FRAME_COLOR = 0xffffff;
}

HpBar::HpBar(int posX, int posY, int width, int height, unsigned int color)
	:
	posX_(posX),
	posY_(posY),
	width_(width),
	height_(height),
	color_(color),
	rate_(1.0f)
{
}

HpBar::~HpBar(void)
{
}

void HpBar::Update(int hp, int maxHp)
{
	// Å‘å’l‚ª0ˆÈ‰º‚¾‚ÆŠ„‚èZ‚Å‚«‚È‚¢‚Ì‚Å‹ó‚É‚·‚é
	if (maxHp <= 0)
	{
		rate_ = 0.0f;
		return;
	}

	rate_ = static_cast<float>(hp) / static_cast<float>(maxHp);

	// 0.0`1.0 ‚Ì”ÍˆÍ‚Éû‚ß‚é
	if (rate_ < 0.0f) rate_ = 0.0f;
	if (rate_ > 1.0f) rate_ = 1.0f;
}

void HpBar::Draw(void) const
{
	int right = posX_ + width_;
	int bottom = posY_ + height_;

	// c‚èHP‚Ì•
	int hpWidth = static_cast<int>(width_ * rate_);

	// ”wŒi
	DrawBox(posX_, posY_, right, bottom, BACK_COLOR, TRUE);

	// c‚èHP
	DrawBox(posX_, posY_, posX_ + hpWidth, bottom, color_, TRUE);

	// ˜g
	DrawBox(posX_, posY_, right, bottom, FRAME_COLOR, FALSE);
}