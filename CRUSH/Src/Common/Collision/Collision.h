#pragma once
#include <DxLib.h>

namespace Collision3D
{
    // 球体同士の衝突判定
    bool HitSpheres(const VECTOR& a, float ar, const VECTOR& b, float br);
    
    // 球体とカプセルの衝突判定
    bool HitSphereCapsule(
        const VECTOR& sphPos, float sphRadius,
        const VECTOR& capA, const VECTOR& capB, float capRadius);
}

namespace Collision2D
{
    // 矩形と点の衝突判定
    bool HitPointRect(
        int x, int y, const RECT& rect);
}