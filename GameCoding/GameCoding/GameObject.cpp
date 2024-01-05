#include "pch.h"
#include "GameObject.h"

GameObject::GameObject(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
	: _device(device)
{
	//_geometry = make_shared<Geometry<VertexTextureData>>();
	_geometry = make_shared<Geometry<VertexColorData>>();
	GeometryHelper::CreateRectangle(_geometry, Color{1.0f, 0.f, 0.f, 1.0f});

	_vertexBuffer = make_shared<VertexBuffer>(device);	
	_vertexBuffer->Create(_geometry->GetVertices());

	_indexBuffer = make_shared<IndexBuffer>(device);
	_indexBuffer->Create(_geometry->GetIndices());

	_vertexShader = make_shared<VertexShader>(device);
	_vertexShader->Create(L"Color.hlsl", "VS", "vs_5_0");

	_inputLayout = make_shared<InputLayout>(device);
	//_inputLayout->Create(VertexTextureData::descs, _vertexShader->GetBlob());
	_inputLayout->Create(VertexColorData::descs, _vertexShader->GetBlob());

	_pixelShader = make_shared<PixelShader>(device);
	_pixelShader->Create(L"Color.hlsl", "PS", "ps_5_0");

	_rasterizerState = make_shared<RasterizerState>(device);
	_rasterizerState->Create();

	_blendState = make_shared<BlendState>(device);	
	_blendState->Create();

	_constantBuffer = make_shared<ConstantBuffer<TransformData>>(device, deviceContext);
	_constantBuffer->Create();
	
	_texture1 = make_shared<Texture>(device);
	_texture1->Create(L"chiikawa.png");

	_samplerState = make_shared<SamplerState>(device);
	_samplerState->Create();
}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
	_localPosition.x += 0.001f;

	Matrix matScale = Matrix::CreateScale(_localScale / 3);
	Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
	matRotation *= Matrix::CreateRotationY(_localRotation.y);
	matRotation *= Matrix::CreateRotationZ(_localRotation.z); // x,y,z 축 회전 행렬 곱하는 순서는 상관 없다. 
	Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

	Matrix matWorld = matScale * matRotation * matTranslation; // SRT
	_transformData.matWorld = matWorld;

	_constantBuffer->CopyData(_transformData);
}

void GameObject::Render(shared_ptr<Pipeline> pipeline)
{// IA - VS - RS - PS - OM
	{
		PipelineInfo info;
		info.inputLayout = _inputLayout;
		info.vertexShader = _vertexShader;
		info.pixelShader = _pixelShader;
		info.rasterizerState = _rasterizerState;
		info.blendState = _blendState;
		pipeline->UpdatePipeline(info);

		pipeline->SetVertexBuffer(_vertexBuffer);
		pipeline->SetIndexBuffer(_indexBuffer);
		pipeline->SetConstantBuffer(0, SS_VertexShader, _constantBuffer);
		pipeline->SetTexture(0, SS_PixelShader, _texture1);
		pipeline->SetSamplerState(0, SS_PixelShader, _samplerState);

		pipeline->DrawIndexed(_geometry->GetIndexCount(), 0, 0);
	}
}
