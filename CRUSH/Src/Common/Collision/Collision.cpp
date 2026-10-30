#include <algorithm>
#include "../Math/Math.h"

#include "Collision.h"

namespace Collision3D
{
    bool HitSpheres(const VECTOR& a, float ar, const VECTOR& b, float br)
    {
        float r = ar + br;
        return Math::Distance(a, b) < r;
    }

    bool HitSphereCapsule(
        const VECTOR& sphPos, float sphRadius,
        const VECTOR& capA, const VECTOR& capB, float capRadius)
    {
        VECTOR ab = VSub(capB, capA);
        VECTOR dir = VNorm(ab);
        float t = VDot(dir, VSub(sphPos, capA));
        t = std::clamp(t, 0.0f, Math::MagnitudeF(ab));

        VECTOR closest = VAdd(capA, VScale(dir, t));
        return HitSpheres(closest, capRadius, sphPos, sphRadius);
    }
}

namespace Collision2D
{
    bool HitPointRect(int x, int y, const RECT& rect)
    {
        return rect.left <= x && x < rect.right &&
            rect.top <= y && y < rect.bottom;
    }
}

namespace CollisionXZ
{
    bool CollisionXZ::HitRectCircle(
        const VECTOR& origin, const VECTOR& forward, float length, float width, 
        const VECTOR& target, float targetRadius)
    {
        // 根元から相手へのベクトル
        VECTOR diff = VSub(target, origin);
        diff.y = 0.0f;

        // 右方向
        VECTOR right = { forward.z, 0.0f, -forward.x };

        // 攻撃から見た座標に直す
        float localX = VDot(diff, right);
        float localZ = VDot(diff, forward);

        // 四角の中で、相手に一番近い点
        float halfWidth = width * 0.5f;
        float nearX = std::clamp(localX, -halfWidth, halfWidth);
        float nearZ = std::clamp(localZ, 0.0f, length);

        // 一番近い点までの距離が、相手の半径以下なら当たり
        float dx = localX - nearX;
        float dz = localZ - nearZ;
        return dx * dx + dz * dz <= targetRadius * targetRadius;
    }

    bool CollisionXZ::HitFanCircle(
        const VECTOR& origin, const VECTOR& forward, float radius, float halfAngleRad, 
        const VECTOR& target, float targetRadius)
    {
        // 根元から相手へのベクトル
        VECTOR diff = VSub(target, origin);
        diff.y = 0.0f;
        float distSq = VSquareSize(diff);

        // 距離が届かなければ外れ
        float reach = radius + targetRadius;
        if (distSq > reach * reach) return false;

        // 相手の円が根元に重なっていれば当たり
        if (distSq <= targetRadius * targetRadius) return true;

        // acosf を使わず、cos 同士で比べる
        float dot = VDot(forward, VNorm(diff));
        return dot >= cosf(halfAngleRad);
    }
}