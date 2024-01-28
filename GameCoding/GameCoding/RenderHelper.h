#pragma once

struct CameraData
{
	Matrix matView = Matrix::Identity;;
	Matrix matProjection = Matrix::Identity; // 항등행렬
};

struct TransformData
{
	Matrix matWorld = Matrix::Identity;;
};

struct AnimationData
{
	Vec2 spriteOffset; 
	Vec2 spriteSize; 
	Vec2 textureSize;
	float useAnimation; 
	float padding; 
};

