#pragma once
#include "Primitive3D.h"

struct MathUtils
{
	//-------------------
	// Point Test
	//-------------------

	// Sphere2Point
	static bool PointInSphere(const Point3D& point, const Sphere3D sphere); 
	static Point3D ClosestPoint(const Sphere3D& sphere, const Point3D& point); 

	// AABB2Point
	static bool PointInAABB(const Point3D& point, const AABB3D& aabb);
	static Point3D ClosestPoint(const AABB3D& aabb, const Point3D& point);

	// OBB2Point
	static bool PointInOBB(const Point3D& point, const OBB3D& obb);
	static Point3D ClosestPoint(const OBB3D& obb, const Point3D& point);

	// Plane2Point
	static bool PointOnPlane(const Point3D& point, const Plane3D& plane);
	static Point3D ClosestPoint(const Plane3D& plane, const Point3D& point);

	// Line2Point
	static bool PointOnLine(const Point3D& point, const Line3D& line);
	static Point3D ClosestPoint(const Line3D& line, const Point3D& point);

	// Ray2Point
	static bool PointOnRay(const Point3D& point, const Ray3D& ray);
	static Point3D ClosestPoint(const Ray3D& ray, const Point3D& point);
};

