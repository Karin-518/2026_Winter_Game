#include <algorithm>

#include "Collision.h"
#include "../Math/Math.h"

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