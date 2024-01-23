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


