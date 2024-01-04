#include "pch.h"
#include "Game.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(HWND hwnd)
{
	_hwnd = hwnd;

	_graphics = make_shared<Graphics>(hwnd); 
	_vertexBuffer = make_shared<VertexBuffer>(_graphics->GetDevice());
	_indexBuffer = make_shared<IndexBuffer>(_graphics->GetDevice());
	_inputLayout = make_shared<InputLayout>(_graphics->GetDevice());
	_geometry = make_shared<Geometry<VertexTextureData>>(); 
	_vertexShader = make_shared<VertexShader>(_graphics->GetDevice()); 
	_pixelShader = make_shared<PixelShader>(_graphics->GetDevice()); 
	_constantBuffer = make_shared<ConstantBuffer<TransformData>>(_graphics->GetDevice(), _graphics->GetDeviceContext());
	_texture1 = make_shared<Texture>(_graphics->GetDevice());

	// VertexData 
	GeometryHelper::CreateRectangle(_geometry);
	// VertexBuffer
	_vertexBuffer->Create(_geometry->GetVertices());
	// IndexBuffer
	_indexBuffer->Create(_geometry->GetIndices());

	_vertexShader->Create(L"Default.hlsl", "VS", "vs_5_0");
	
	_inputLayout->Create(VertexTextureData::descs, _vertexShader->GetBlob());

	_pixelShader->Create(L"Default.hlsl", "PS", "ps_5_0");

	CreateRasterizerState();
	CreateSamplerState();
	CreateBlendState();

	_texture1->Create(L"chiikawa.png");
	
	_constantBuffer->Create(); 
}

void Game::Update()
{
	// Scale Rotation Translation

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

void Game::Render()
{
	_graphics->RenderBegin();

	// IA - VS - RS - PS - OM
	{
		uint32 stride = sizeof(VertexTextureData);
		uint32 offset = 0; 

		auto _deviceContext = _graphics->GetDeviceContext(); 

		// IA
		// 코딩하는게 아니라 세팅하는 거
		_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer->GetComPtr().GetAddressOf(), &stride, &offset);
		_deviceContext->IASetIndexBuffer(_indexBuffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
		_deviceContext->IASetInputLayout(_inputLayout->GetComPtr().Get()); // 어떻게 생겨먹은 애인지 묘사해줘야 해
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 삼각형으로 인지를 해달라 부탁 

		// VS
		// 코딩 가능
		_deviceContext->VSSetShader(_vertexShader->GetComPtr().Get(), nullptr, 0);
		_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer->GetComPtr().GetAddressOf());

		// RS
		_deviceContext->RSSetState(_rasterizerState.Get()); 
			
		// PS
		_deviceContext->PSSetShader(_pixelShader->GetComPtr().Get(), nullptr, 0);
		_deviceContext->PSSetShaderResources(0, 1, _texture1->GetComPtr().GetAddressOf());
		// _deviceContext->PSSetShaderResources(1, 1, _shaderResourceView2.GetAddressOf()); 
		_deviceContext->PSSetSamplers(0, 1, _samplerState.GetAddressOf()); 

		// OM
		_deviceContext->OMSetBlendState(_blendState.Get(), nullptr, 0xFFFFFFFF); 

		// _deviceContext->Draw(_vertices.size(), 0); 
		_deviceContext->DrawIndexed(_geometry->GetIndexCount(), 0, 0);
	}

	_graphics->RenderEnd();
}

void Game::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC desc; 
	ZeroMemory(&desc, sizeof(desc)); 
	desc.FillMode = D3D11_FILL_SOLID; 
	desc.CullMode = D3D11_CULL_BACK; 
	desc.FrontCounterClockwise = false; // 이것들은 실습을 하면서 얘기를 해볼거야. 

	HRESULT hr = _graphics->GetDevice()->CreateRasterizerState(&desc, _rasterizerState.GetAddressOf());
	CHECK(hr); 
}

void Game::CreateSamplerState()
{
	D3D11_SAMPLER_DESC desc; 
	ZeroMemory(&desc, sizeof(desc));
	// 여기다가 마음에 드는 옵션들을 세팅해 보면된다. 
	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.BorderColor[0] = 1; 
	desc.BorderColor[1] = 0; 
	desc.BorderColor[2] = 0; 
	desc.BorderColor[3] = 1; 
	// 여기부턴 별로 안중요해서 그냥 블로그에서 본 아무 값
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS; 
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; 
	desc.MaxAnisotropy = 16; 
	desc.MaxLOD = FLT_MAX; 
	desc.MinLOD = FLT_MIN; 
	desc.MipLODBias = 0.0f; 

	_graphics->GetDevice()->CreateSamplerState(&desc, _samplerState.GetAddressOf());
}

void Game::CreateBlendState()
{
	D3D11_BLEND_DESC desc; 
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC)); 
	desc.AlphaToCoverageEnable = false; 
	desc.IndependentBlendEnable = false; 

	desc.RenderTarget[0].BlendEnable = true; 
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; 
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; 
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; 
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE; 
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO; 
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; 
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; 

	HRESULT hr = _graphics->GetDevice()->CreateBlendState(&desc, _blendState.GetAddressOf());
	CHECK(hr); 
}



