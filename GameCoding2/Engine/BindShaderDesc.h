#pragma once
#include "ConstantBuffer.h"

class Shader; 

struct GlobalDesc 
{
	Matrix V = Matrix::Identity; 
	Matrix P = Matrix::Identity; 
	Matrix VP = Matrix::Identity; 
	Matrix VInv = Matrix::Identity; 
};

struct TransformDesc
{
	Matrix W = Matrix::Identity; 
};

// Light
struct LightDesc
{
	Color ambient = Color(1.f, 1.f, 1.f, 1.f);
	Color diffuse = Color(1.f, 1.f, 1.f, 1.f);
	Color specular = Color(1.f, 1.f, 1.f, 1.f);
	Color emissive = Color(1.f, 1.f, 1.f, 1.f);

	Vec3 direction;
	float padding0;
};

struct MaterialDesc
{
	Color ambient = Color(0.f, 0.f, 0.f, 1.f);
	Color diffuse = Color(1.f, 1.f, 1.f, 1.f);
	Color specular = Color(0.f, 0.f, 0.f, 1.f);
	Color emissive = Color(0.f, 0.f, 0.f, 1.f);
};

// Bone
#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500
#define MAX_MODEL_INSTANCE 500

struct BoneDesc
{
	Matrix transform[MAX_MODEL_TRANSFORMS]; 
};

// Animation
struct KeyframeDesc
{
	int32 animIndex = 0; // 현재 실행되는 애니메이션 번호
	uint32 currFrame = 0;
	uint32 nextFrame = 0; 
	float ratio = 0.f; 
	float sumTime = 0.f; 
	float speed = 1.f; 
	Vec2 padding; 
};

struct TweenDesc
{
	// 꼼꼼하게 안챙기면 엉뚱하게 돌아갈 수 있으니까
	TweenDesc()
	{
		curr.animIndex = 0; 
		next.animIndex = -1; 
	}

	// 경우에 따라서 다음 애니메이션이 다 완료되었습니다. 라는 함수를 파줘서 그걸 호출하면 좋을 거 같으니까
	// 편리하게 초기값으로 밀어주는 함수
	void ClearNextAnim()
	{
		next.animIndex = -1; 
		next.currFrame = 0; 
		next.nextFrame = 0;
		next.sumTime = 0; 
		tweenSumTime = 0; 
		tweenRatio = 0;
	}

	float tweenDuration = 1.0f; 
	float tweenRatio = 0.f; // 이전 상태에서 다음 상태로 넘어갈 때의 비율
	float tweenSumTime = 0.f; // 경과된 시간
	float padding = 0.f; 
	KeyframeDesc curr; 
	KeyframeDesc next; 
};

struct InstancedTweenDesc
{
	TweenDesc tweens[MAX_MODEL_INSTANCE]; 
};

// 눈을 뿌리고 싶을 때 어떻게 뿌리고 싶을지와 관련된 정보
struct SnowBillboardDesc
{
	Color color = Color(1, 1, 1, 1);

	Vec3 velocity = Vec3(0, -5, 0); // 속도
	float drawDistance = 0;

	Vec3 origin = Vec3(0, 0, 0);
	float turbulence = 5; // 흔들리는 강도

	Vec3 extent = Vec3(0, 0, 0);
	float time;
};
