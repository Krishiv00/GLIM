#include <cmath>
#include <limits>

#include "GLIM/System/Shapes.hpp"

using namespace gl;

namespace Helper {
    inline float Dot(Vector2f a, Vector2f b) {
        return a.x * b.x + a.y * b.y;
    }

    inline float Dot(Vector3f a, Vector3f b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    inline float Cross(Vector2f a, Vector2f b) {
        return a.x * b.y - a.y * b.x;
    }

    inline Vector3f Cross(Vector3f a, Vector3f b) {
        return Vector3f(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    inline float LengthSq(Vector2f v) {
        return v.x * v.x + v.y * v.y;
    }

    inline float LengthSq(Vector3f v) {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    }

    inline float Length(Vector2f v) {
        return std::sqrt(LengthSq(v));
    }

    inline float Length(Vector3f v) {
        return std::sqrt(LengthSq(v));
    }

    inline Vector2f Normalize(Vector2f v) {
        const float len = Length(v);
        return len > 1e-6f ? v / len : Vector2f(0, 0);
    }

    inline Vector3f Normalize(Vector3f v) {
        const float len = Length(v);
        return len > 1e-6f ? v / len : Vector3f(0, 0, 0);
    }

    inline float DistSq(Vector2f a, Vector2f b) {
        return LengthSq(a - b);
    }

    inline float DistSq(Vector3f a, Vector3f b) {
        return LengthSq(a - b);
    }

    inline bool PointInRect(Vector2f p, const Rectangle& r) {
        return (
            p.x >= r.position.x && p.x <= r.position.x + r.size.x
            && p.y >= r.position.y && p.y <= r.position.y + r.size.y
        );
    }

    inline float Sign(Vector2f p, Vector2f a, Vector2f b) {
        return (p.x - b.x) * (a.y - b.y) - (a.x - b.x) * (p.y - b.y);
    }

    inline bool PointInTriangle(Vector2f p, const Triangle& t) {
        const float d1 = Sign(p, t.p0, t.p1);
        const float d2 = Sign(p, t.p1, t.p2);
        const float d3 = Sign(p, t.p2, t.p0);

        const bool neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        const bool pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(neg && pos);
    }

    inline bool PointInCuboid(Vector3f p, const Cuboid& c) {
        return (
            p.x >= c.position.x && p.x <= c.position.x + c.size.x &&
            p.y >= c.position.y && p.y <= c.position.y + c.size.y &&
            p.z >= c.position.z && p.z <= c.position.z + c.size.z
        );
    }

    inline bool PointInCone(Vector3f p, const Cone& k) {
        const Vector3f axis = k.tip - k.base;
        const float lenSq = LengthSq(axis);
        if (lenSq < 1e-6f) return false;

        const float len = std::sqrt(lenSq);
        const float t = Dot(p - k.base, axis) / lenSq;
        if (t < 0.f || t > 1.f) return false;

        const Vector3f closest = k.base + axis * t;
        const float radiusAtT = k.radius * (1.f - t);
        return DistSq(p, closest) <= radiusAtT * radiusAtT;
    }

    inline bool PointInCylinder(Vector3f p, const Cylinder& cy) {
        const Vector3f axis = cy.tip - cy.base;
        const float lenSq = LengthSq(axis);
        if (lenSq < 1e-6f) return DistSq(p, cy.base) <= cy.radius * cy.radius;

        const float t = Dot(p - cy.base, axis) / lenSq;
        if (t < 0.f || t > 1.f) return false;

        const Vector3f closest = cy.base + axis * t;
        return DistSq(p, closest) <= cy.radius * cy.radius;
    }

    inline bool PointInPolygon(Vector2f p, const Polygon2D& poly) {
        if (poly.vertices.size() < 3) return false;

        bool inside = false;
        const std::size_t n = poly.vertices.size();

        for (std::size_t i = 0, j = n - 1; i < n; j = i++) {
            const Vector2f vi = poly.vertices[i];
            const Vector2f vj = poly.vertices[j];

            if (
                ((vi.y > p.y) != (vj.y > p.y)) &&
                (p.x < (vj.x - vi.x) * (p.y - vi.y) / (vj.y - vi.y) + vi.x)
            ) {
                inside = !inside;
            }
        }

        return inside;
    }

    inline bool PointInPolygon(Vector3f p, const Polygon3D& poly) {
        if (poly.vertices.size() < 3) return false;

        const float normalLenSq = LengthSq(poly.normal);
        if (normalLenSq < 1e-6f) return false;

        const float dist = Dot(p - poly.vertices[0], poly.normal);
        if (std::abs(dist) > 1e-4f) return false;

        const Vector3f n = poly.normal;
        const float ax = std::abs(n.x), ay = std::abs(n.y), az = std::abs(n.z);

        const std::size_t size = poly.vertices.size();
        bool inside = false;

        auto project = [&](Vector3f v) -> std::pair<float, float> {
            if (az >= ax && az >= ay) return {v.x, v.y};
            if (ay >= ax && ay >= az) return {v.x, v.z};
            return {v.y, v.z};
            };

        auto [px, py] = project(p);

        for (std::size_t i = 0, j = size - 1; i < size; j = i++) {
            auto [vix, viy] = project(poly.vertices[i]);
            auto [vjx, vjy] = project(poly.vertices[j]);

            if (
                ((viy > py) != (vjy > py)) &&
                (px < (vjx - vix) * (py - viy) / (vjy - viy) + vix)
            ) {
                inside = !inside;
            }
        }

        return inside;
    }

    inline bool LineSegmentIntersect(Vector2f p1, Vector2f p2, Vector2f p3, Vector2f p4) {
        const Vector2f d1 = p2 - p1;
        const Vector2f d2 = p4 - p3;

        const float denom = Cross(d1, d2);
        if (std::abs(denom) < 1e-6f) return false;

        const Vector2f d3 = p3 - p1;
        const float t = Cross(d3, d2) / denom;
        const float u = Cross(d3, d1) / denom;

        return t >= 0 && t <= 1 && u >= 0 && u <= 1;
    }

    inline float PointToSegmentDistSq(Vector2f p, Vector2f a, Vector2f b) {
        const Vector2f ab = b - a;
        const float lenSq = Helper::LengthSq(ab);

        if (lenSq < 1e-6f) return DistSq(p, a);

        const float t = std::max(0.f, std::min(1.f, Dot(p - a, ab) / lenSq));
        const Vector2f projection = a + ab * t;

        return DistSq(p, projection);
    }

    inline float PointToSegmentDistSq(Vector3f p, Vector3f a, Vector3f b) {
        const Vector3f ab = b - a;
        const float lenSq = LengthSq(ab);

        if (lenSq < 1e-6f) return DistSq(p, a);

        const float t = std::max(0.f, std::min(1.f, Dot(p - a, ab) / lenSq));
        const Vector3f projection = a + ab * t;
        return DistSq(p, projection);
    }

    inline bool LineIntersectsPolygon(Vector3f p0, Vector3f p1, const Polygon3D& poly) {
        if (poly.vertices.size() < 3) return false;

        const float normalLenSq = LengthSq(poly.normal);
        if (normalLenSq < 1e-6f) return false;

        const Vector3f d = p1 - p0;
        const float dDotN = Dot(d, poly.normal);
        const float originDot = Dot(poly.vertices[0] - p0, poly.normal);

        if (std::abs(dDotN) < 1e-6f) {
            if (std::abs(originDot) > 1e-4f) return false;
            return PointInPolygon(p0, poly) || PointInPolygon(p1, poly);
        }

        const float t = originDot / dDotN;
        if (t < 0.f || t > 1.f) return false;

        return PointInPolygon(p0 + d * t, poly);
    }

    inline bool CuboidIntersectsLine(const Cuboid& c, Vector3f p0, Vector3f p1) {
        const Vector3f d = p1 - p0;
        float tMin = 0.f, tMax = 1.f;

        auto testSlab = [&](float orig, float dir, float bMin, float bMax) -> bool {
            if (std::abs(dir) < 1e-6f) return orig >= bMin && orig <= bMax;
            float t0 = (bMin - orig) / dir;
            float t1 = (bMax - orig) / dir;
            if (t0 > t1) std::swap(t0, t1);
            tMin = std::max(tMin, t0);
            tMax = std::min(tMax, t1);
            return tMin <= tMax;
            };

        return (
            testSlab(p0.x, d.x, c.position.x, c.position.x + c.size.x) &&
            testSlab(p0.y, d.y, c.position.y, c.position.y + c.size.y) &&
            testSlab(p0.z, d.z, c.position.z, c.position.z + c.size.z)
        );
    }

    inline Sphere ConeBoundingSphere(const Cone& k) {
        const Vector3f center = (k.base + k.tip) * 0.5f;
        const float halfLen = std::sqrt(DistSq(k.base, k.tip)) * 0.5f;
        return Sphere(center, std::sqrt(halfLen * halfLen + k.radius * k.radius));
    }

    inline bool CircleLineIntersect(const Circle& c, Vector2f a, Vector2f b) {
        return PointToSegmentDistSq(c.position, a, b) <= c.radius * c.radius;
    }

    inline void GetPolygonEdges(const Polygon2D& poly, std::vector<std::pair<Vector2f, Vector2f>>& edges) {
        edges.clear();

        const std::size_t n = poly.vertices.size();

        for (std::size_t i = 0; i < n; ++i) {
            edges.emplace_back(poly.vertices[i], poly.vertices[(i + 1) % n]);
        }
    }

    inline void GetPolygonEdges(const Polygon3D& poly, std::vector<std::pair<Vector3f, Vector3f>>& edges) {
        edges.clear();

        const std::size_t n = poly.vertices.size();

        for (std::size_t i = 0; i < n; ++i) {
            edges.emplace_back(poly.vertices[i], poly.vertices[(i + 1) % n]);
        }
    }

    inline float SegmentToSegmentDistSq(Vector3f p0, Vector3f p1, Vector3f q0, Vector3f q1) {
        const Vector3f d1 = p1 - p0;
        const Vector3f d2 = q1 - q0;
        const Vector3f r = p0 - q0;

        const float a = Dot(d1, d1);
        const float e = Dot(d2, d2);
        const float f = Dot(d2, r);

        float s;
        float t;

        if (a < 1e-6f && e < 1e-6f) {
            return Dot(r, r);
        }

        if (a < 1e-6f) {
            s = 0.f;
            t = std::max(0.f, std::min(1.f, f / e));
        } else {
            const float c = Dot(d1, r);
            if (e < 1e-6f) {
                t = 0.f;
                s = std::max(0.f, std::min(1.f, -c / a));
            } else {
                const float b = Dot(d1, d2);
                const float denom = a * e - b * b;

                if (std::abs(denom) > 1e-6f) {
                    s = std::max(0.f, std::min(1.f, (b * f - c * e) / denom));
                } else {
                    s = 0.f;
                }

                t = (b * s + f) / e;

                if (t < 0.f) {
                    t = 0.f;
                    s = std::max(0.f, std::min(1.f, -c / a));
                } else if (t > 1.f) {
                    t = 1.f;
                    s = std::max(0.f, std::min(1.f, (b - c) / a));
                }
            }
        }

        const Vector3f closest1 = p0 + d1 * s;
        const Vector3f closest2 = q0 + d2 * t;

        return DistSq(closest1, closest2);
    }

    struct SegmentProjection {
        float t;
        float axial;
        float radialSq;
    };

    inline SegmentProjection ProjectOntoSegment(Vector3f p, Vector3f base, Vector3f tip) {
        const Vector3f axis = tip - base;
        const float lenSq = LengthSq(axis);

        SegmentProjection result{};

        if (lenSq < 1e-6f) {
            result.t = 0.f;
            result.axial = 0.f;
            result.radialSq = DistSq(p, base);

            return result;
        }

        const float axial = Dot(p - base, axis) / std::sqrt(lenSq);
        const float len = std::sqrt(lenSq);

        result.t = std::max(0.f, std::min(1.f, axial / len));
        result.axial = axial;

        const Vector3f closest = base + axis * (result.t);
        result.radialSq = DistSq(p, closest);
        return result;
    }
}

bool gl::Intersects(const Point2D& p, const Rectangle& r) {
    return Helper::PointInRect(p.position, r);
}

bool gl::Intersects(const Rectangle& r, const Point2D& p) {
    return Intersects(p, r);
}

bool gl::Intersects(const Point2D& p, const Circle& c) {
    return Helper::DistSq(p.position, c.position) <= c.radius * c.radius;
}

bool gl::Intersects(const Circle& c, const Point2D& p) {
    return Intersects(p, c);
}

bool gl::Intersects(const Point2D& p, const Triangle& t) {
    return Helper::PointInTriangle(p.position, t);
}

bool gl::Intersects(const Triangle& t, const Point2D& p) {
    return Intersects(p, t);
}

bool gl::Intersects(const Point2D& a, const Point2D& b) {
    return Helper::DistSq(a.position, b.position) < 1e-6f;
}

bool gl::Intersects(const Point2D& p, const Polygon2D& poly) {
    return Helper::PointInPolygon(p.position, poly);
}

bool gl::Intersects(const Polygon2D& poly, const Point2D& p) {
    return Intersects(p, poly);
}

bool gl::Intersects(const Point2D& p, const Line2D& l) {
    return Helper::PointToSegmentDistSq(p.position, l.p0, l.p1) < 1e-6f;
}

bool gl::Intersects(const Line2D& l, const Point2D& p) {
    return Intersects(p, l);
}

bool gl::Intersects(const Point3D& a, const Point3D& b) {
    return Helper::DistSq(a.position, b.position) < 1e-6f;
}

bool gl::Intersects(const Point3D& p, const Sphere& s) {
    return Helper::DistSq(p.position, s.position) <= s.radius * s.radius;
}

bool gl::Intersects(const Sphere& s, const Point3D& p) {
    return Intersects(p, s);
}

bool gl::Intersects(const Point3D& p, const Cuboid& c) {
    return Helper::PointInCuboid(p.position, c);
}

bool gl::Intersects(const Cuboid& c, const Point3D& p) {
    return Intersects(p, c);
}

bool gl::Intersects(const Point3D& p, const Cone& k) {
    return Helper::PointInCone(p.position, k);
}

bool gl::Intersects(const Cone& k, const Point3D& p) {
    return Intersects(p, k);
}

bool gl::Intersects(const Point3D& p, const Cylinder& cy) {
    return Helper::PointInCylinder(p.position, cy);
}

bool gl::Intersects(const Cylinder& cy, const Point3D& p) {
    return Intersects(p, cy);
}

bool gl::Intersects(const Point3D& p, const Polygon3D& poly) {
    return Helper::PointInPolygon(p.position, poly);
}

bool gl::Intersects(const Polygon3D& poly, const Point3D& p) {
    return Intersects(p, poly);
}

bool gl::Intersects(const Point3D& p, const Line3D& l) {
    return Helper::PointToSegmentDistSq(p.position, l.p0, l.p1) < 1e-6f;
}

bool gl::Intersects(const Line3D& l, const Point3D& p) {
    return Intersects(p, l);
}

bool gl::Intersects(const Circle& a, const Circle& b) {
    const float radSum = a.radius + b.radius;
    return Helper::DistSq(a.position, b.position) <= radSum * radSum;
}

bool gl::Intersects(const Circle& c, const Rectangle& r) {
    const Vector2f closest(
        std::max(r.position.x, std::min(c.position.x, r.position.x + r.size.x)),
        std::max(r.position.y, std::min(c.position.y, r.position.y + r.size.y))
    );

    return Helper::DistSq(c.position, closest) <= c.radius * c.radius;
}

bool gl::Intersects(const Rectangle& r, const Circle& c) {
    return Intersects(c, r);
}

bool gl::Intersects(const Circle& c, const Triangle& t) {
    if (Helper::PointInTriangle(c.position, t)) return true;

    const float r2 = c.radius * c.radius;
    if (Helper::PointToSegmentDistSq(c.position, t.p0, t.p1) <= r2) return true;
    if (Helper::PointToSegmentDistSq(c.position, t.p1, t.p2) <= r2) return true;
    if (Helper::PointToSegmentDistSq(c.position, t.p2, t.p0) <= r2) return true;

    return false;
}

bool gl::Intersects(const Triangle& t, const Circle& c) {
    return Intersects(c, t);
}

bool gl::Intersects(const Circle& c, const Polygon2D& poly) {
    if (Helper::PointInPolygon(c.position, poly)) return true;

    std::vector<std::pair<Vector2f, Vector2f>> edges;
    Helper::GetPolygonEdges(poly, edges);

    const float r2 = c.radius * c.radius;
    for (const auto& [a, b] : edges) {
        if (Helper::PointToSegmentDistSq(c.position, a, b) <= r2) return true;
    }

    return false;
}

bool gl::Intersects(const Polygon2D& poly, const Circle& c) {
    return Intersects(c, poly);
}

bool gl::Intersects(const Circle& c, const Line2D& l) {
    return Helper::CircleLineIntersect(c, l.p0, l.p1);
}

bool gl::Intersects(const Line2D& l, const Circle& c) {
    return Intersects(c, l);
}

bool gl::Intersects(const Sphere& a, const Sphere& b) {
    const float radSum = a.radius + b.radius;
    return Helper::DistSq(a.position, b.position) <= radSum * radSum;
}

bool gl::Intersects(const Sphere& s, const Cuboid& c) {
    const Vector3f closest(
        std::max(c.position.x, std::min(s.position.x, c.position.x + c.size.x)),
        std::max(c.position.y, std::min(s.position.y, c.position.y + c.size.y)),
        std::max(c.position.z, std::min(s.position.z, c.position.z + c.size.z))
    );
    return Helper::DistSq(s.position, closest) <= s.radius * s.radius;
}

bool gl::Intersects(const Cuboid& c, const Sphere& s) {
    return Intersects(s, c);
}

bool gl::Intersects(const Sphere& s, const Cone& k) {
    // Closest point on cone axis to sphere center
    const float distSqToAxis = Helper::PointToSegmentDistSq(s.position, k.base, k.tip);

    // Conservative: check if sphere hits the bounding cylinder-ish region
    // then refine by checking cone surface
    const Vector3f axis = k.tip - k.base;
    const float axisLen = Helper::Length(axis);

    if (axisLen < 1e-6f) {
        return Helper::DistSq(s.position, k.base) <= s.radius * s.radius;
    }

    if (Helper::PointInCone(s.position, k)) return true;

    const float t = std::max(0.f, std::min(1.f,
        Helper::Dot(s.position - k.base, axis) / (axisLen * axisLen)
    ));

    const float radiusAtT = k.radius * (1.f - t);
    const Vector3f axisPoint = k.base + axis * t;
    const Vector3f toPoint = s.position - axisPoint;
    const float radDist = Helper::Length(toPoint);

    if (radDist < 1e-6f) {
        return true;
    }

    // Closest point on cone surface at this axial slice
    const Vector3f surfacePoint = axisPoint + (toPoint / radDist) * radiusAtT;
    return Helper::DistSq(s.position, surfacePoint) <= s.radius * s.radius;
}

bool gl::Intersects(const Cone& k, const Sphere& s) {
    return Intersects(s, k);
}

bool gl::Intersects(const Sphere& s, const Cylinder& cy) {
    return Helper::PointToSegmentDistSq(s.position, cy.base, cy.tip) <= (s.radius + cy.radius) * (s.radius + cy.radius);
}

bool gl::Intersects(const Cylinder& cy, const Sphere& s) {
    return Intersects(s, cy);
}

bool gl::Intersects(const Sphere& s, const Polygon3D& poly) {
    if (poly.vertices.size() < 3) return false;

    const float normalLenSq = Helper::LengthSq(poly.normal);
    if (normalLenSq < 1e-6f) return false;

    const float dist = Helper::Dot(s.position - poly.vertices[0], poly.normal) / std::sqrt(normalLenSq);
    if (std::abs(dist) > s.radius) return false;

    // Project sphere center onto polygon plane
    const Vector3f projected = s.position - poly.normal * (Helper::Dot(s.position - poly.vertices[0], poly.normal) / normalLenSq);

    if (Helper::PointInPolygon(projected, poly)) return true;

    std::vector<std::pair<Vector3f, Vector3f>> edges;
    Helper::GetPolygonEdges(poly, edges);

    const float r2 = s.radius * s.radius;
    for (const auto& [a, b] : edges) {
        if (Helper::PointToSegmentDistSq(s.position, a, b) <= r2) return true;
    }

    return false;
}

bool gl::Intersects(const Polygon3D& poly, const Sphere& s) {
    return Intersects(s, poly);
}

bool gl::Intersects(const Sphere& s, const Line3D& l) {
    return Helper::PointToSegmentDistSq(s.position, l.p0, l.p1) <= s.radius * s.radius;
}

bool gl::Intersects(const Line3D& l, const Sphere& s) {
    return Intersects(s, l);
}

bool gl::Intersects(const Rectangle& a, const Rectangle& b) {
    return (
        a.position.x < b.position.x + b.size.x &&
        a.position.x + a.size.x > b.position.x &&
        a.position.y < b.position.y + b.size.y &&
        a.position.y + a.size.y > b.position.y
    );
}

bool gl::Intersects(const Rectangle& r, const Triangle& t) {
    if (Helper::PointInRect(t.p0, r)) return true;
    if (Helper::PointInRect(t.p1, r)) return true;
    if (Helper::PointInRect(t.p2, r)) return true;

    const Vector2f corners[4] = {
        r.position,
        r.position + Vector2f(r.size.x, 0),
        r.position + r.size,
        r.position + Vector2f(0, r.size.y)
    };

    for (int i = 0; i < 4; ++i) {
        if (Helper::PointInTriangle(corners[i], t)) return true;
    }

    const Vector2f edges[4][2] = {
        {r.position, r.position + Vector2f(r.size.x, 0)},
        {r.position + Vector2f(r.size.x, 0), r.position + r.size},
        {r.position + r.size, r.position + Vector2f(0, r.size.y)},
        {r.position + Vector2f(0, r.size.y), r.position}
    };

    for (int i = 0; i < 4; ++i) {
        if (Helper::LineSegmentIntersect(t.p0, t.p1, edges[i][0], edges[i][1])) return true;
        if (Helper::LineSegmentIntersect(t.p1, t.p2, edges[i][0], edges[i][1])) return true;
        if (Helper::LineSegmentIntersect(t.p2, t.p0, edges[i][0], edges[i][1])) return true;
    }

    return false;
}

bool gl::Intersects(const Triangle& t, const Rectangle& r) {
    return Intersects(r, t);
}

bool gl::Intersects(const Rectangle& r, const Polygon2D& poly) {
    for (const auto& v : poly.vertices) {
        if (Helper::PointInRect(v, r)) return true;
    }

    const Vector2f corners[4] = {
        r.position,
        r.position + Vector2f(r.size.x, 0),
        r.position + r.size,
        r.position + Vector2f(0, r.size.y)
    };

    for (int i = 0; i < 4; ++i) {
        if (Helper::PointInPolygon(corners[i], poly)) return true;
    }

    std::vector<std::pair<Vector2f, Vector2f>> polyEdges;
    Helper::GetPolygonEdges(poly, polyEdges);

    const Vector2f rectEdges[4][2] = {
        {r.position, r.position + Vector2f(r.size.x, 0)},
        {r.position + Vector2f(r.size.x, 0), r.position + r.size},
        {r.position + r.size, r.position + Vector2f(0, r.size.y)},
        {r.position + Vector2f(0, r.size.y), r.position}
    };

    for (int i = 0; i < 4; ++i) {
        for (const auto& [a, b] : polyEdges) {
            if (Helper::LineSegmentIntersect(rectEdges[i][0], rectEdges[i][1], a, b)) {
                return true;
            }
        }
    }

    return false;
}

bool gl::Intersects(const Polygon2D& poly, const Rectangle& r) {
    return Intersects(r, poly);
}

bool gl::Intersects(const Rectangle& r, const Line2D& l) {
    if (Helper::PointInRect(l.p0, r) || Helper::PointInRect(l.p1, r)) return true;

    const Vector2f edges[4][2] = {
        {r.position, r.position + Vector2f(r.size.x, 0)},
        {r.position + Vector2f(r.size.x, 0), r.position + r.size},
        {r.position + r.size, r.position + Vector2f(0, r.size.y)},
        {r.position + Vector2f(0, r.size.y), r.position}
    };

    for (int i = 0; i < 4; ++i) {
        if (Helper::LineSegmentIntersect(l.p0, l.p1, edges[i][0], edges[i][1])) {
            return true;
        }
    }

    return false;
}

bool gl::Intersects(const Line2D& l, const Rectangle& r) {
    return Intersects(r, l);
}

bool gl::Intersects(const Triangle& a, const Triangle& b) {
    if (Helper::PointInTriangle(a.p0, b) || Helper::PointInTriangle(a.p1, b) || Helper::PointInTriangle(a.p2, b)) return true;
    if (Helper::PointInTriangle(b.p0, a) || Helper::PointInTriangle(b.p1, a) || Helper::PointInTriangle(b.p2, a)) return true;

    const Vector2f edgesA[3][2] = {{a.p0, a.p1}, {a.p1, a.p2}, {a.p2, a.p0}};
    const Vector2f edgesB[3][2] = {{b.p0, b.p1}, {b.p1, b.p2}, {b.p2, b.p0}};

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (Helper::LineSegmentIntersect(edgesA[i][0], edgesA[i][1], edgesB[j][0], edgesB[j][1])) {
                return true;
            }
        }
    }

    return false;
}

bool gl::Intersects(const Cuboid& a, const Cuboid& b) {
    return (
        a.position.x < b.position.x + b.size.x &&
        a.position.x + a.size.x > b.position.x &&
        a.position.y < b.position.y + b.size.y &&
        a.position.y + a.size.y > b.position.y &&
        a.position.z < b.position.z + b.size.z &&
        a.position.z + a.size.z > b.position.z
    );
}

bool gl::Intersects(const Cuboid& c, const Cone& k) {
    // Broad phase: bounding sphere of cone vs cuboid
    if (!Intersects(Helper::ConeBoundingSphere(k), c)) return false;

    // Check cone tip and base center
    if (Helper::PointInCuboid(k.tip, c))  return true;
    if (Helper::PointInCuboid(k.base, c)) return true;

    // Check cuboid corners against cone
    const Vector3f corners[8] = {
        c.position,
        c.position + Vector3f(c.size.x, 0, 0),
        c.position + Vector3f(0, c.size.y, 0),
        c.position + Vector3f(0, 0, c.size.z),
        c.position + Vector3f(c.size.x, c.size.y, 0),
        c.position + Vector3f(c.size.x, 0, c.size.z),
        c.position + Vector3f(0, c.size.y, c.size.z),
        c.position + c.size
    };

    for (const auto& corner : corners) {
        if (Helper::PointInCone(corner, k)) return true;
    }

    // Check cone axis segment against cuboid
    return Helper::CuboidIntersectsLine(c, k.base, k.tip);
}

bool gl::Intersects(const Cone& k, const Cuboid& c) {
    return Intersects(c, k);
}

bool gl::Intersects(const Cuboid& c, const Cylinder& cy) {
    // Closest point on cylinder axis to cuboid, then check radius
    const Vector3f closest(
        std::max(c.position.x, std::min(cy.base.x, c.position.x + c.size.x)),
        std::max(c.position.y, std::min(cy.base.y, c.position.y + c.size.y)),
        std::max(c.position.z, std::min(cy.base.z, c.position.z + c.size.z))
    );

    // Clamp closest point onto cylinder axis
    const float distSqAxisToBox = Helper::PointToSegmentDistSq(closest, cy.base, cy.tip);
    if (distSqAxisToBox <= cy.radius * cy.radius) return true;

    // Check cylinder axis endpoints inside cuboid
    if (Helper::PointInCuboid(cy.base, c) || Helper::PointInCuboid(cy.tip, c)) return true;

    // Check cuboid corners inside cylinder
    const Vector3f corners[8] = {
        c.position,
        c.position + Vector3f(c.size.x, 0, 0),
        c.position + Vector3f(0, c.size.y, 0),
        c.position + Vector3f(0, 0, c.size.z),
        c.position + Vector3f(c.size.x, c.size.y, 0),
        c.position + Vector3f(c.size.x, 0, c.size.z),
        c.position + Vector3f(0, c.size.y, c.size.z),
        c.position + c.size
    };

    for (const auto& corner : corners) {
        if (Helper::PointInCylinder(corner, cy)) return true;
    }

    return false;
}

bool gl::Intersects(const Cylinder& cy, const Cuboid& c) {
    return Intersects(c, cy);
}

bool gl::Intersects(const Cuboid& c, const Polygon3D& poly) {
    for (const auto& v : poly.vertices) {
        if (Helper::PointInCuboid(v, c)) return true;
    }

    const Vector3f corners[8] = {
        c.position,
        c.position + Vector3f(c.size.x, 0, 0),
        c.position + Vector3f(0, c.size.y, 0),
        c.position + Vector3f(0, 0, c.size.z),
        c.position + Vector3f(c.size.x, c.size.y, 0),
        c.position + Vector3f(c.size.x, 0, c.size.z),
        c.position + Vector3f(0, c.size.y, c.size.z),
        c.position + c.size
    };

    for (const auto& corner : corners) {
        if (Helper::PointInPolygon(corner, poly)) return true;
    }

    std::vector<std::pair<Vector3f, Vector3f>> polyEdges;
    Helper::GetPolygonEdges(poly, polyEdges);
    for (const auto& [a, b] : polyEdges) {
        if (Helper::CuboidIntersectsLine(c, a, b)) return true;
    }

    const Vector3f boxEdges[12][2] = {
        {corners[0], corners[1]}, {corners[0], corners[2]}, {corners[0], corners[3]},
        {corners[4], corners[1]}, {corners[4], corners[2]}, {corners[4], corners[7]},
        {corners[5], corners[1]}, {corners[5], corners[3]}, {corners[5], corners[7]},
        {corners[6], corners[2]}, {corners[6], corners[3]}, {corners[6], corners[7]}
    };

    for (const auto& [a, b] : boxEdges) {
        if (Helper::LineIntersectsPolygon(a, b, poly)) return true;
    }

    return false;
}

bool gl::Intersects(const Polygon3D& poly, const Cuboid& c) {
    return Intersects(c, poly);
}

bool gl::Intersects(const Cuboid& c, const Line3D& l) {
    if (Helper::PointInCuboid(l.p0, c) || Helper::PointInCuboid(l.p1, c)) return true;
    return Helper::CuboidIntersectsLine(c, l.p0, l.p1);
}

bool gl::Intersects(const Line3D& l, const Cuboid& c) {
    return Intersects(c, l);
}

bool gl::Intersects(const Triangle& t, const Polygon2D& poly) {
    if (Helper::PointInPolygon(t.p0, poly) || Helper::PointInPolygon(t.p1, poly) || Helper::PointInPolygon(t.p2, poly)) {
        return true;
    }

    for (const auto& v : poly.vertices) {
        if (Helper::PointInTriangle(v, t)) return true;
    }

    std::vector<std::pair<Vector2f, Vector2f>> polyEdges;
    Helper::GetPolygonEdges(poly, polyEdges);

    const Vector2f triEdges[3][2] = {{t.p0, t.p1}, {t.p1, t.p2}, {t.p2, t.p0}};

    for (int i = 0; i < 3; ++i) {
        for (const auto& [a, b] : polyEdges) {
            if (Helper::LineSegmentIntersect(triEdges[i][0], triEdges[i][1], a, b)) {
                return true;
            }
        }
    }

    return false;
}

bool gl::Intersects(const Polygon2D& poly, const Triangle& t) {
    return Intersects(t, poly);
}

bool gl::Intersects(const Triangle& t, const Line2D& l) {
    if (Helper::PointInTriangle(l.p0, t) || Helper::PointInTriangle(l.p1, t)) return true;

    if (Helper::LineSegmentIntersect(l.p0, l.p1, t.p0, t.p1)) return true;
    if (Helper::LineSegmentIntersect(l.p0, l.p1, t.p1, t.p2)) return true;
    if (Helper::LineSegmentIntersect(l.p0, l.p1, t.p2, t.p0)) return true;

    return false;
}

bool gl::Intersects(const Line2D& l, const Triangle& t) {
    return Intersects(t, l);
}

bool gl::Intersects(const Cone& a, const Cone& b) {
    // Broad phase via bounding spheres
    const Sphere sa = Helper::ConeBoundingSphere(a);
    const Sphere sb = Helper::ConeBoundingSphere(b);
    if (!Intersects(sa, sb)) return false;

    if (Helper::PointInCone(a.tip, b) || Helper::PointInCone(a.base, b)) return true;
    if (Helper::PointInCone(b.tip, a) || Helper::PointInCone(b.base, a)) return true;

    // Approximate: sweep both axes with bounding cylinders at closest approach
    const float distSq = Helper::SegmentToSegmentDistSq(a.base, a.tip, b.base, b.tip);
    return distSq <= (a.radius + b.radius) * (a.radius + b.radius);
}

bool gl::Intersects(const Cone& k, const Cylinder& cy) {
    if (Helper::PointInCone(cy.base, k) || Helper::PointInCone(cy.tip, k)) return true;
    if (Helper::PointInCylinder(k.base, cy) || Helper::PointInCylinder(k.tip, cy)) return true;

    const float distSq = Helper::SegmentToSegmentDistSq(k.base, k.tip, cy.base, cy.tip);
    return distSq <= (k.radius + cy.radius) * (k.radius + cy.radius);
}

bool gl::Intersects(const Cylinder& cy, const Cone& k) {
    return Intersects(k, cy);
}

bool gl::Intersects(const Cone& k, const Polygon3D& poly) {
    // Conservative: bounding sphere vs polygon
    const Sphere bs = Helper::ConeBoundingSphere(k);
    if (!Intersects(bs, poly)) return false;

    if (Helper::PointInPolygon(k.tip, poly))  return true;
    if (Helper::PointInPolygon(k.base, poly)) return true;

    for (const auto& v : poly.vertices) {
        if (Helper::PointInCone(v, k)) return true;
    }

    std::vector<std::pair<Vector3f, Vector3f>> edges;
    Helper::GetPolygonEdges(poly, edges);

    for (const auto& [a, b] : edges) {
        const Vector3f axis = k.tip - k.base;
        const float axisLen = Helper::Length(axis);
        if (axisLen < 1e-6f) continue;

        for (int i = 0; i <= 8; ++i) {
            const float edgeT = static_cast<float>(i) / 8.f;
            const Vector3f pt = a + (b - a) * edgeT;
            if (Helper::PointInCone(pt, k)) return true;
        }
    }

    return false;
}

bool gl::Intersects(const Polygon3D& poly, const Cone& k) {
    return Intersects(k, poly);
}

bool gl::Intersects(const Cone& k, const Line3D& l) {
    if (Helper::PointInCone(l.p0, k) || Helper::PointInCone(l.p1, k)) return true;

    for (int i = 1; i <= 16; ++i) {
        const float t = static_cast<float>(i) / 17.f;
        const Vector3f pt = l.p0 + (l.p1 - l.p0) * t;
        if (Helper::PointInCone(pt, k)) return true;
    }

    return false;
}

bool gl::Intersects(const Line3D& l, const Cone& k) {
    return Intersects(k, l);
}

bool gl::Intersects(const Cylinder& a, const Cylinder& b) {
    const float distSq = Helper::SegmentToSegmentDistSq(a.base, a.tip, b.base, b.tip);
    const float radSum = a.radius + b.radius;
    return distSq <= radSum * radSum;
}

bool gl::Intersects(const Cylinder& cy, const Polygon3D& poly) {
    for (const auto& v : poly.vertices) {
        if (Helper::PointInCylinder(v, cy)) return true;
    }

    if (Helper::PointInPolygon(cy.base, poly) || Helper::PointInPolygon(cy.tip, poly)) return true;

    std::vector<std::pair<Vector3f, Vector3f>> edges;
    Helper::GetPolygonEdges(poly, edges);

    const float r2 = cy.radius * cy.radius;
    for (const auto& [a, b] : edges) {
        if (Helper::SegmentToSegmentDistSq(cy.base, cy.tip, a, b) <= r2) return true;
    }

    return false;
}

bool gl::Intersects(const Polygon3D& poly, const Cylinder& cy) {
    return Intersects(cy, poly);
}

bool gl::Intersects(const Cylinder& cy, const Line3D& l) {
    return Helper::SegmentToSegmentDistSq(cy.base, cy.tip, l.p0, l.p1) <= cy.radius * cy.radius;
}

bool gl::Intersects(const Line3D& l, const Cylinder& cy) {
    return Intersects(cy, l);
}

bool gl::Intersects(const Polygon2D& a, const Polygon2D& b) {
    for (const auto& v : a.vertices) {
        if (Helper::PointInPolygon(v, b)) return true;
    }

    for (const auto& v : b.vertices) {
        if (Helper::PointInPolygon(v, a)) return true;
    }

    std::vector<std::pair<Vector2f, Vector2f>> edgesA, edgesB;
    Helper::GetPolygonEdges(a, edgesA);
    Helper::GetPolygonEdges(b, edgesB);

    for (const auto& [a0, a1] : edgesA) {
        for (const auto& [b0, b1] : edgesB) {
            if (Helper::LineSegmentIntersect(a0, a1, b0, b1)) {
                return true;
            }
        }
    }

    return false;
}

bool gl::Intersects(const Polygon2D& poly, const Line2D& l) {
    if (Helper::PointInPolygon(l.p0, poly) || Helper::PointInPolygon(l.p1, poly)) return true;

    std::vector<std::pair<Vector2f, Vector2f>> edges;
    Helper::GetPolygonEdges(poly, edges);

    for (const auto& [a, b] : edges) {
        if (Helper::LineSegmentIntersect(l.p0, l.p1, a, b)) return true;
    }

    return false;
}

bool gl::Intersects(const Line2D& l, const Polygon2D& poly) {
    return Intersects(poly, l);
}

bool gl::Intersects(const Polygon3D& a, const Polygon3D& b) {
    for (const auto& v : a.vertices) {
        if (Helper::PointInPolygon(v, b)) return true;
    }
    for (const auto& v : b.vertices) {
        if (Helper::PointInPolygon(v, a)) return true;
    }

    std::vector<std::pair<Vector3f, Vector3f>> edgesA, edgesB;
    Helper::GetPolygonEdges(a, edgesA);
    Helper::GetPolygonEdges(b, edgesB);

    for (const auto& [a0, a1] : edgesA) {
        if (Helper::LineIntersectsPolygon(a0, a1, b)) return true;
    }
    
    for (const auto& [b0, b1] : edgesB) {
        if (Helper::LineIntersectsPolygon(b0, b1, a)) return true;
    }

    return false;
}

bool gl::Intersects(const Polygon3D& poly, const Line3D& l) {
    if (Helper::PointInPolygon(l.p0, poly) || Helper::PointInPolygon(l.p1, poly)) return true;
    return Helper::LineIntersectsPolygon(l.p0, l.p1, poly);
}

bool gl::Intersects(const Line3D& l, const Polygon3D& poly) {
    return Intersects(poly, l);
}

bool gl::Intersects(const Line2D& a, const Line2D& b) {
    return Helper::LineSegmentIntersect(a.p0, a.p1, b.p0, b.p1);
}

bool gl::Intersects(const Line3D& a, const Line3D& b) {
    return Helper::SegmentToSegmentDistSq(a.p0, a.p1, b.p0, b.p1) < 1e-6f;
}