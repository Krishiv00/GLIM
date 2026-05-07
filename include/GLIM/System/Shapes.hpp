#pragma once

#include <vector>

#include "GLIM/System/Vector2.hpp"
#include "GLIM/System/Vector3.hpp"

namespace gl {
    struct Rectangle {
        Vector2f position;
        Vector2f size;

        Rectangle() = default;
        Rectangle(Vector2f pos, Vector2f sz) : position(pos), size(sz) {}
    };

    struct Cuboid {
        Vector3f position;
        Vector3f size;

        Cuboid() = default;
        Cuboid(Vector3f pos, Vector3f sz) : position(pos), size(sz) {}
    };

    struct Circle {
        Vector2f position;
        float radius;

        Circle() = default;
        Circle(Vector2f pos, float r) : position(pos), radius(r) {}
    };

    struct Sphere {
        Vector3f position;
        float radius;

        Sphere() = default;
        Sphere(Vector3f pos, float r) : position(pos), radius(r) {}
    };

    struct Triangle {
        Vector2f p0, p1, p2;

        Triangle() = default;
        Triangle(Vector2f a, Vector2f b, Vector2f c) : p0(a), p1(b), p2(c) {}
    };

    struct Cone {
        Vector3f base;
        Vector3f tip;
        float radius; // base radius

        Cone() = default;
        Cone(Vector3f base, Vector3f tip, float r) : base(base), tip(tip), radius(r) {}
    };

    struct Cylinder {
        Vector3f base;
        Vector3f tip;
        float radius;

        Cylinder() = default;
        Cylinder(Vector3f base, Vector3f tip, float r) : base(base), tip(tip), radius(r) {}
    };

    struct Polygon2D {
        std::vector<Vector2f> vertices;

        Polygon2D() = default;
        Polygon2D(std::vector<Vector2f> verts) : vertices(std::move(verts)) {}
    };

    struct Polygon3D {
        std::vector<Vector3f> vertices;
        Vector3f normal;

        Polygon3D() = default;
        Polygon3D(std::vector<Vector3f> verts, Vector3f n) : vertices(std::move(verts)), normal(n) {}
    };

    struct Point2D {
        Vector2f position;

        Point2D() = default;
        Point2D(Vector2f pos) : position(pos) {}
    };

    struct Point3D {
        Vector3f position;

        Point3D() = default;
        Point3D(Vector3f pos) : position(pos) {}
    };

    struct Line2D {
        Vector2f p0, p1;

        Line2D() = default;
        Line2D(Vector2f a, Vector2f b) : p0(a), p1(b) {}
    };

    struct Line3D {
        Vector3f p0, p1;

        Line3D() = default;
        Line3D(Vector3f a, Vector3f b) : p0(a), p1(b) {}
    };

    [[nodiscard]]
    bool Intersects(const Point2D& p, const Rectangle& r);

    [[nodiscard]]
    bool Intersects(const Rectangle& r, const Point2D& p);

    [[nodiscard]]
    bool Intersects(const Point2D& p, const Circle& c);

    [[nodiscard]]
    bool Intersects(const Circle& c, const Point2D& p);

    [[nodiscard]]
    bool Intersects(const Point2D& p, const Triangle& t);

    [[nodiscard]]
    bool Intersects(const Triangle& t, const Point2D& p);

    [[nodiscard]]
    bool Intersects(const Point2D& a, const Point2D& b);

    [[nodiscard]]
    bool Intersects(const Point2D& p, const Polygon2D& poly);

    [[nodiscard]]
    bool Intersects(const Polygon2D& poly, const Point2D& p);

    [[nodiscard]]
    bool Intersects(const Point2D& p, const Line2D& l);

    [[nodiscard]]
    bool Intersects(const Line2D& l, const Point2D& p);

    [[nodiscard]]
    bool Intersects(const Circle& a, const Circle& b);

    [[nodiscard]]
    bool Intersects(const Circle& c, const Rectangle& r);

    [[nodiscard]]
    bool Intersects(const Rectangle& r, const Circle& c);

    [[nodiscard]]
    bool Intersects(const Circle& c, const Triangle& t);

    [[nodiscard]]
    bool Intersects(const Triangle& t, const Circle& c);

    [[nodiscard]]
    bool Intersects(const Circle& c, const Polygon2D& poly);

    [[nodiscard]]
    bool Intersects(const Polygon2D& poly, const Circle& c);

    [[nodiscard]]
    bool Intersects(const Circle& c, const Line2D& l);

    [[nodiscard]]
    bool Intersects(const Line2D& l, const Circle& c);

    [[nodiscard]]
    bool Intersects(const Rectangle& a, const Rectangle& b);

    [[nodiscard]]
    bool Intersects(const Rectangle& r, const Triangle& t);

    [[nodiscard]]
    bool Intersects(const Triangle& t, const Rectangle& r);

    [[nodiscard]]
    bool Intersects(const Rectangle& r, const Polygon2D& poly);

    [[nodiscard]]
    bool Intersects(const Polygon2D& poly, const Rectangle& r);

    [[nodiscard]]
    bool Intersects(const Rectangle& r, const Line2D& l);

    [[nodiscard]]
    bool Intersects(const Line2D& l, const Rectangle& r);

    [[nodiscard]]
    bool Intersects(const Triangle& a, const Triangle& b);

    [[nodiscard]]
    bool Intersects(const Triangle& t, const Polygon2D& poly);

    [[nodiscard]]
    bool Intersects(const Polygon2D& poly, const Triangle& t);

    [[nodiscard]]
    bool Intersects(const Triangle& t, const Line2D& l);

    [[nodiscard]]
    bool Intersects(const Line2D& l, const Triangle& t);

    [[nodiscard]]
    bool Intersects(const Polygon2D& a, const Polygon2D& b);

    [[nodiscard]]
    bool Intersects(const Polygon2D& poly, const Line2D& l);

    [[nodiscard]]
    bool Intersects(const Line2D& l, const Polygon2D& poly);

    [[nodiscard]]
    bool Intersects(const Line2D& a, const Line2D& b);

    [[nodiscard]]
    bool Intersects(const Point3D& a, const Point3D& b);

    [[nodiscard]]
    bool Intersects(const Point3D& p, const Sphere& s);

    [[nodiscard]]
    bool Intersects(const Sphere& s, const Point3D& p);

    [[nodiscard]]
    bool Intersects(const Point3D& p, const Cuboid& c);

    [[nodiscard]]
    bool Intersects(const Cuboid& c, const Point3D& p);

    [[nodiscard]]
    bool Intersects(const Point3D& p, const Cone& c);

    [[nodiscard]]
    bool Intersects(const Cone& c, const Point3D& p);

    [[nodiscard]]
    bool Intersects(const Point3D& p, const Cylinder& c);

    [[nodiscard]]
    bool Intersects(const Cylinder& c, const Point3D& p);

    [[nodiscard]]
    bool Intersects(const Point3D& p, const Polygon3D& poly);

    [[nodiscard]]
    bool Intersects(const Polygon3D& poly, const Point3D& p);

    [[nodiscard]]
    bool Intersects(const Point3D& p, const Line3D& l);

    [[nodiscard]]
    bool Intersects(const Line3D& l, const Point3D& p);

    [[nodiscard]]
    bool Intersects(const Sphere& a, const Sphere& b);

    [[nodiscard]]
    bool Intersects(const Sphere& s, const Cuboid& c);

    [[nodiscard]]
    bool Intersects(const Cuboid& c, const Sphere& s);

    [[nodiscard]]
    bool Intersects(const Sphere& s, const Cone& c);

    [[nodiscard]]
    bool Intersects(const Cone& c, const Sphere& s);

    [[nodiscard]]
    bool Intersects(const Sphere& s, const Cylinder& c);

    [[nodiscard]]
    bool Intersects(const Cylinder& c, const Sphere& s);

    [[nodiscard]]
    bool Intersects(const Sphere& s, const Polygon3D& poly);

    [[nodiscard]]
    bool Intersects(const Polygon3D& poly, const Sphere& s);

    [[nodiscard]]
    bool Intersects(const Sphere& s, const Line3D& l);

    [[nodiscard]]
    bool Intersects(const Line3D& l, const Sphere& s);

    [[nodiscard]]
    bool Intersects(const Cuboid& a, const Cuboid& b);

    [[nodiscard]]
    bool Intersects(const Cuboid& c, const Cone& k);

    [[nodiscard]]
    bool Intersects(const Cone& k, const Cuboid& c);

    [[nodiscard]]
    bool Intersects(const Cuboid& c, const Cylinder& cy);

    [[nodiscard]]
    bool Intersects(const Cylinder& cy, const Cuboid& c);

    [[nodiscard]]
    bool Intersects(const Cuboid& c, const Polygon3D& poly);

    [[nodiscard]]
    bool Intersects(const Polygon3D& poly, const Cuboid& c);

    [[nodiscard]]
    bool Intersects(const Cuboid& c, const Line3D& l);

    [[nodiscard]]
    bool Intersects(const Line3D& l, const Cuboid& c);

    [[nodiscard]]
    bool Intersects(const Cone& a, const Cone& b);

    [[nodiscard]]
    bool Intersects(const Cone& k, const Cylinder& c);

    [[nodiscard]]
    bool Intersects(const Cylinder& c, const Cone& k);

    [[nodiscard]]
    bool Intersects(const Cone& k, const Polygon3D& poly);

    [[nodiscard]]
    bool Intersects(const Polygon3D& poly, const Cone& k);

    [[nodiscard]]
    bool Intersects(const Cone& k, const Line3D& l);

    [[nodiscard]]
    bool Intersects(const Line3D& l, const Cone& k);

    [[nodiscard]]
    bool Intersects(const Cylinder& a, const Cylinder& b);

    [[nodiscard]]
    bool Intersects(const Cylinder& cy, const Polygon3D& poly);

    [[nodiscard]]
    bool Intersects(const Polygon3D& poly, const Cylinder& cy);

    [[nodiscard]]
    bool Intersects(const Cylinder& cy, const Line3D& l);

    [[nodiscard]]
    bool Intersects(const Line3D& l, const Cylinder& cy);

    [[nodiscard]]
    bool Intersects(const Polygon3D& a, const Polygon3D& b);

    [[nodiscard]]
    bool Intersects(const Polygon3D& poly, const Line3D& l);

    [[nodiscard]]
    bool Intersects(const Line3D& l, const Polygon3D& poly);

    [[nodiscard]]
    bool Intersects(const Line3D& a, const Line3D& b);
}