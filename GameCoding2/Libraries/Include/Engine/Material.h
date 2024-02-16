#pragma once
#include "ResourceBase.h"
class Material :  public ResourceBase
{
	using Super = ResourceBase; 
public: 
	Material();
	virtual ~Material();

	shared_ptr<Shader> GetShader() { return _shader; }

	MaterialDesc& GetMaterialDesc() { return _desc; }
	shared_ptr<Texture> GetDiffuseMap() { return _diffuseMap; }
	shared_ptr<Texture> GetNormalMap() { return _normalMap; }
	shared_ptr<Texture> GetSpecularMap() { return _specularMap; }

	void SetShader(shared_ptr<Shader> shader);
	void SetDiffuseMap(shared_ptr<Texture> diffuseMap) { _diffuseMap = diffuseMap; }
	void SetNormalMap(shared_ptr<Texture> normalMap) { _normalMap = normalMap; }
	void SetSpecularMap(shared_ptr<Texture> specularMap) { _specularMap = specularMap; }

	void Update();

	shared_ptr<Material> Clone();

private: 
	friend class MeshRenderer; 

	MaterialDesc _desc; // Material의 각기 색상들이 들어있다.

	shared_ptr<Shader> _shader;
	shared_ptr<Texture> _diffuseMap;
	shared_ptr<Texture> _normalMap;
	shared_ptr<Texture> _specularMap;
	// 텍스쳐를 여기에 세팅한 다음에 
	// 그 쉐이더에 따라서 각기 접근할 수 있는 애들을 캐싱해서 
	// 나중에 material을 이용해 세팅을 할 때 그걸 이용해서 바로 하자가 컨셉이다.
	ComPtr<ID3DX11EffectShaderResourceVariable> _diffuseEffectBuffer;
	ComPtr<ID3DX11EffectShaderResourceVariable> _normalEffectBuffer;
	ComPtr<ID3DX11EffectShaderResourceVariable> _specularEffectBuffer;
};

