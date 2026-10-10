#pragma once
#include <DxLib.h>

namespace Draw
{
    void DrawLineDir(const VECTOR& pos, const VECTOR& dir, int color, float len = 50.0f);
    void DrawAxis(const VECTOR& pos, const MATRIX& rot, float len = 50.0f);
}
