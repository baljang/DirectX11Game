#pragma once
#include "Types.h"

struct Vertex
{
	Vec3 position; 
	// Color color; 
	Vec2 uv; 
};

struct TransformData
{
	Vec3 offset; 
	float dummy;  
	// 컨스턴트 버퍼를 만들 때는 16바이트 정렬을 해야하기 때문에 
};