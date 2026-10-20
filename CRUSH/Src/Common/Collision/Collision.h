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

namespace CollisionXZ
{
    /// <summary>
    /// 四角と円の判定
    /// </summary>
    /// <param name="origin">四角の手前の辺の真ん中</param>
    /// <param name="forward">前方向</param>
    /// <param name="length"></param>
    /// <param name="width"></param>
    /// <param name="target"></param>
    /// <param name="targetRadius"></param>
    /// <returns></returns>
    bool HitRectCircle(
        const VECTOR& origin, const VECTOR& forward, float length, float width,
        const VECTOR& target, float targetRadius);

    /// <summary>
    /// 扇と円の判定
    /// </summary>
    /// <param name="origin">扇の根元</param>
    /// <param name="forward">扇の真ん中の向き</param>
    /// <param name="radius">扇の半径</param>
    /// <param name="halfAngleRad">真ん中から端までの角度</param>
    /// <param name="target"></param>
    /// <param name="targetRadius"></param>
    /// <returns></returns>
    bool HitFanCircle(
        const VECTOR& origin, const VECTOR& forward, float radius, float halfAngleRad,
        const VECTOR& target, float targetRadius);
}