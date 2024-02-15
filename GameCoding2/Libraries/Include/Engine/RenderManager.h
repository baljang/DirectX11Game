#pragma once
#include "ConstantBuffer.h"

class Shader; 

struct GlobalDesc 
{
	Matrix V = Matrix::Identity; 
	Matrix P = Matrix::Identity; 
	Matrix VP = Matrix::Identity; 
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


class RenderManager
{
	DECLARE_SINGLE(RenderManager); 

public:
	// 셰이더 마다 연결해줘야 되는게 달라질테니까 셰이더를 세팅하는 부분
	void Init(shared_ptr<Shader> shader); 
	void Update();

	// 정보를 밀어 넣기 위해
	void PushGlobalData(const Matrix& view, const Matrix& projection); 
	void PushTransformData(const TransformDesc& desc); 
	void PushLightData(const LightDesc& desc);
	void PushMaterialData(const MaterialDesc& desc);

private: 
	shared_ptr<Shader> _shader;

	GlobalDesc _globalDesc;
	// 프레임마다 한번만 세팅하는 정보들

	shared_ptr<ConstantBuffer<GlobalDesc>> _globalBuffer;
	// 상수버퍼라는걸 만들어서 그 정보를 GPU에다가 전달 해줬었다.	

	ComPtr<ID3DX11EffectConstantBuffer> _globalEffectBuffer;
	// 쉐이더에게 이 버퍼를 사용하라 했을 때 Effect11를 사용했는데 
	// ComPtr<ID3DX11EffectConstantBuffer> 이 아이를 사용하게 될 것이다. 
	// 지금까지는 이걸 사용하지 않았다.
	// MeshRenderer::Render에서 GetMatrix같이 Get~ 함수를 호출할 때 커서를 올려 보면
	// 반환값의 타입이 ComPtr<ID3DX11EffectMatrixVariable> 이었다.
	// 이제는 ID3DX11EffectConstantBuffer을 가져올 것이다. 
	// _shader->GetConstantBuffer("버퍼이름")를 호출해서 가져올건데 
	// 매번마다 Get을 하는 것 보다는 갖고 온 정보를 캐싱해서 그걸 ComPtr로 들고 있어서 하는게 더 효율적이다. 

	// 어떠한 정보를 넣고 싶으면 이 삼총사가 마련이 된다고 보면 된다. 
	// 마찬가지로 한번 더 복붙해서 

	TransformDesc _transformDesc;
	shared_ptr<ConstantBuffer<TransformDesc>> _transformBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> _transformEffectBuffer;

	LightDesc _lightDesc;
	shared_ptr<ConstantBuffer<LightDesc>> _lightBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> _lightEffectBuffer;

	MaterialDesc _materialDesc;
	shared_ptr<ConstantBuffer<MaterialDesc>> _materialBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> _materialEffectBuffer;
};

