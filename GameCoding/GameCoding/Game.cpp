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

	//_graphics = make_shared<Graphics>(hwnd); 
	_graphics = new Graphics(hwnd); 
	_vertexBuffer = new VertexBuffer(_graphics->GetDevice()); 
	_indexBuffer = new IndexBuffer(_graphics->GetDevice()); 
	_inputLayout = new InputLayout(_graphics->GetDevice());

	CreateGeometry(); 
	CreateVS(); 
	CreateInputLayout(); 
	CreatePS(); 

	CreateRasterizerState();
	CreateSamplerState();
	CreateBlendState();

	CreateSRV(); 
	CreateContantBuffer(); 
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

	D3D11_MAPPED_SUBRESOURCE subResource; 
	ZeroMemory(&subResource, sizeof(subResource));

	_graphics->GetDeviceContext()->Map(_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	::memcpy(subResource.pData, &_transformData, sizeof(_transformData)); 
	_graphics->GetDeviceContext()->Unmap(_constantBuffer.Get(), 0);
}

void Game::Render()
{
	_graphics->RenderBegin();

	// IA - VS - RS - PS - OM
	{
		uint32 stride = sizeof(Vertex); 
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
		_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0); 
		_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf()); 

		// RS
		_deviceContext->RSSetState(_rasterizerState.Get()); 
			
		// PS
		_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0); 
		_deviceContext->PSSetShaderResources(0, 1, _shaderResourceView.GetAddressOf()); 
		_deviceContext->PSSetShaderResources(1, 1, _shaderResourceView2.GetAddressOf()); 
		_deviceContext->PSSetSamplers(0, 1, _samplerState.GetAddressOf()); 

		// OM
		_deviceContext->OMSetBlendState(_blendState.Get(), nullptr, 0xFFFFFFFF); 

		// _deviceContext->Draw(_vertices.size(), 0); 
		_deviceContext->DrawIndexed(_indices.size(), 0, 0);
	}

	_graphics->RenderEnd();
}

void Game::CreateGeometry()
{
	// VertexData - CPU 메모리에 있는 거 
	// 1 3
	// 0 2
	{
		_vertices.resize(4);

		_vertices[0].position = Vec3(-0.5f, -0.5f, 0.f); 
		_vertices[0].uv = Vec2(0.f, 1.f); 
		// _vertices[0].color = Color(1.f, 0.f, 0.f, 1.f); 

		_vertices[1].position = Vec3(-0.5f, 0.5f, 0.f);
		_vertices[1].uv = Vec2(0.f, 0.f);
		// _vertices[1].color = Color(1.f, 0.f, 0.f, 1.f);

		_vertices[2].position = Vec3(0.5f, -0.5f, 0.f);
		_vertices[2].uv = Vec2(1.f, 1.f);
		// _vertices[2].color = Color(1.f, 0.f, 0.f, 1.f);

		_vertices[3].position = Vec3(0.5f, 0.5f, 0.f);
		_vertices[3].uv = Vec2(1.f, 0.f);
		// _vertices[3].color = Color(1.f, 0.f, 0.f, 1.f);
	}

	// VertexBuffer
	{
		_vertexBuffer->Create(_vertices);
	}

	// Index
	{
		_indices = { 0, 1, 2, 2, 1, 3 }; // 시계방향 골랐으면 시계방향 유지해야 한다. 삼각형 2개 만들어 줬다.
	}

	// IndexBuffer
	{
		_indexBuffer->Create(_indices); 
	}

}

void Game::CreateInputLayout()
{
	vector<D3D11_INPUT_ELEMENT_DESC> layout
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12바이트 부터 컬러가 시작 offset
	};

	_inputLayout->Create(layout, _vsBlob);
}

// 셰이더를 로드해서 Blob이라는 걸 만들어준 다음에 그 Blob에 있는 정보들을 이용해서 Vertex shader를 만들어주면 최종적으로 완료가 되면 _vertexShader가 채워진다. 
void Game::CreateVS()
{
	LoadShaderFromFile(L"Default.hlsl", "VS", "vs_5_0", _vsBlob); // _vsBlob이 채워지게 된다. 
	
	HRESULT hr = _graphics->GetDevice()->CreateVertexShader(_vsBlob->GetBufferPointer(),
		_vsBlob->GetBufferSize(), nullptr, _vertexShader.GetAddressOf()); 
	CHECK(hr); 
}

void Game::CreatePS()
{
	LoadShaderFromFile(L"Default.hlsl", "PS", "ps_5_0", _psBlob); // _psBlob이 채워지게 된다. 

	HRESULT hr = _graphics->GetDevice()->CreatePixelShader(_psBlob->GetBufferPointer(),
		_psBlob->GetBufferSize(), nullptr, _pixelShader.GetAddressOf());
	CHECK(hr);
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

void Game::CreateSRV()
{
	DirectX::TexMetadata md; 
	DirectX::ScratchImage img; 
	HRESULT hr = ::LoadFromWICFile(L"chiikawa.png", WIC_FLAGS_NONE, &md, img); 
	CHECK(hr); 

	hr = ::CreateShaderResourceView(_graphics->GetDevice().Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView.GetAddressOf());
	CHECK(hr); 

	hr = ::LoadFromWICFile(L"hachiware.png", WIC_FLAGS_NONE, &md, img);
	CHECK(hr);

	hr = ::CreateShaderResourceView(_graphics->GetDevice().Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView2.GetAddressOf());
	CHECK(hr);
}

void Game::CreateContantBuffer()
{
	D3D11_BUFFER_DESC desc; 
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DYNAMIC; // CPU_Write + GPU_Read
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; 
	desc.ByteWidth = sizeof(TransformData); 
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU도 접근할 수 있따. 

	HRESULT hr = _graphics->GetDevice()->CreateBuffer(&desc, nullptr, _constantBuffer.GetAddressOf());
	CHECK(hr); 
}

void Game::LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob)
{
	const uint32 compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION; 

	// pch.h에서 d3dcompliler.h를 include했기에 지원이 되는 함수 
	HRESULT hr = ::D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		name.c_str(),
		version.c_str(),
		compileFlag,
		0,
		blob.GetAddressOf(),
		nullptr); 

	CHECK(hr); 	
}
