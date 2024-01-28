#pragma once
#include "ResourceBase.h"

class Texture : public ResourceBase
{
	using Super = ResourceBase; 
public:
	Texture(ComPtr<ID3D11Device> device);
	~Texture(); 

	ComPtr<ID3D11ShaderResourceView> GetComPtr() { return _shaderResourceView; } 

	void Create(const wstring& path); // 경로를 건내주면 찾아서 뭔가 만들어주는 코드를 실행한다.

	Vec2 GetSize() { return _size; } 

private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView;
	Vec2 _size = { 0.f, 0.f }; 
};

