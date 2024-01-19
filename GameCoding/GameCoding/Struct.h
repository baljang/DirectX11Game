#pragma once
#include "Types.h"

//struct Vertex
//{
//	Vec3 position; 
//	// Color color; 
//	Vec2 uv; 
//};

struct CameraData
{
	Matrix matView = Matrix::Identity;;
	Matrix matProjection = Matrix::Identity; // 항등행렬
};

struct TransformData
{
	Matrix matWorld = Matrix::Identity;;
};