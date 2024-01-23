#pragma once
#include "Component.h"

class MeshRenderer :  public Component
{
	using Super = Component; 

public: 
	MeshRenderer(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext);
	virtual ~MeshRenderer(); 

	virtual void Update() override;

private: 
	ComPtr<ID3D11Device> _device;

	friend class RenderManager; 
	// Mesh
	shared_ptr<Geometry<VertexTextureData>> _geometry;
	// shared_ptr<Geometry<VertexColorData>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer;
	shared_ptr<IndexBuffer> _indexBuffer;
	shared_ptr<InputLayout> _inputLayout;

	// Material
	shared_ptr<VertexShader> _vertexShader;
	shared_ptr<PixelShader> _pixelShader;
	shared_ptr<Texture> _texture1;


private:



};

