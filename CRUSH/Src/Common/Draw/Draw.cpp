#include "Draw.h"
#include "../Math/Math.h"

namespace
{
    // 先端の球の半径と分割数
    constexpr float TIP_RADIUS = 5.0f;
    constexpr int TIP_DIV_NUM = 5;

    // 軸の色
    constexpr unsigned int COLOR_RIGHT = 0xff0000;
    constexpr unsigned int COLOR_UP = 0x00ff00;
    constexpr unsigned int COLOR_FORWARD = 0x0000ff;
}

namespace Draw
{
    void DrawLineDir(const VECTOR& pos, const VECTOR& dir, unsigned int color, float len)
    {
        const VECTOR n = Math::Normalize(dir);
        const VECTOR tip = VAdd(pos, VScale(n, len));

        DrawLine3D(VAdd(pos, VScale(n, -len)), tip, color);
        DrawSphere3D(tip, TIP_RADIUS, TIP_DIV_NUM, color, color, true);
    }

    void DrawAxis(const VECTOR& pos, const MATRIX& rot, float len)
    {
        // 回転だけを掛けて(平行移動は無視して)、右・上・前の向きを取り出す
        DrawLineDir(pos, VTransformSR(Math::DIR_R, rot), COLOR_RIGHT, len);
        DrawLineDir(pos, VTransformSR(Math::DIR_U, rot), COLOR_UP, len);
        DrawLineDir(pos, VTransformSR(Math::DIR_F, rot), COLOR_FORWARD, len);
    }
}