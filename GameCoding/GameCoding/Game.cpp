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
	_width = GWinSizeX;
	_height = GWinSizeY;

	// TODO

	//_device->AddRef(); 
	//_device->Release(); 
	CreateDeviceAndSwapChain(); 
	CreateRenderTargetView(); 
	SetViewport(); 

	CreateGeometry(); 
	CreateVS(); 
	CreateInputLayout(); 
	CreatePS(); 

	CreateSRV(); 
}

void Game::Update()
{
}

void Game::Render()
{
	RenderBegin(); 

	// IA - VS - RS - PS - OM
	{
		uint32 stride = sizeof(Vertex); 
		uint32 offset = 0; 

		// IA
		// 코딩하는게 아니라 세팅하는 거
		_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset); 
		_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0); 
		_deviceContext->IASetInputLayout(_inputLayout.Get()); // 어떻게 생겨먹은 애인지 묘사해줘야 해
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 삼각형으로 인지를 해달라 부탁 

		// VS
		// 코딩 가능
		_deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0); 

		// RS
		// 일단 스킵

		// PS
		_deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0); 
		_deviceContext->PSSetShaderResources(0, 1, _shaderResourceView.GetAddressOf()); 
		_deviceContext->PSSetShaderResources(1, 1, _shaderResourceView2.GetAddressOf()); 

		// OM

		// _deviceContext->Draw(_vertices.size(), 0); 
		_deviceContext->DrawIndexed(_indices.size(), 0, 0);
	}

	RenderEnd(); 
}

void Game::RenderBegin()
{
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr); 
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor); 
	_deviceContext->RSSetViewports(1, &_viewport); 
}

void Game::RenderEnd()
{ 
	// RenderBegin에서 후면 버퍼에 그려달라 요청한 거고
	// 여기서는 전면 버퍼에 복사를 해달라, 그리고 화면에 출력을 해달라 
	// [ ] <- [ ]
	HRESULT hr = _swapChain->Present(1, 0); // 그린 거를 제출하겠다.
	CHECK(hr); 
}

// 
void Game::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc; // 안에 또 구조체가 있기 때문에 복잡하다. Device and SwapChain을 하고 있다에 집중하는게 좋다.
	ZeroMemory(&desc, sizeof(desc)); // desc를 0으로 밀어 버린다. ::memset(&desc, 0, sizeof(desc))이랑 마찬가지. 0으로 민 다음 필요한 걸로 채워줘야 해
	{
		desc.BufferDesc.Width = _width; 
		desc.BufferDesc.Height = _height; // 화면이 800x600이니 버퍼도 똑같이 800x600으로 만들어 주는 거
		desc.BufferDesc.RefreshRate.Numerator = 60; // 화면 주사율
		desc.BufferDesc.RefreshRate.Denominator = 1; // 이름으로 때려 맞춰 보는 연습도 재밌다. 
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 8비트x4짜리로 만들어 주겠다
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // 지금 단계에선 신경 안써도 되는 옵션.
		desc.SampleDesc.Count = 1; // 계단 현상 줄일 때 숫자 늘일 수 있어.
		desc.SampleDesc.Quality = 0; 
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 최종 결과물을 그려주는 역할로 사용하겠다는 옵션
		desc.BufferCount = 1; // 후면 버퍼 개수
		desc.OutputWindow = _hwnd; // 윈도우 핸들
		desc.Windowed = true; 
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; 
	}

	//_device.Get();  // ComPtr내부에서 관리하는 ID3D11Device를 꺼내고 싶을 때, 즉 ID3D11Device* 얘를 꺼내고 싶을 때 
	//_device.GetAddressOf(); //ID3D11Device*의 주소값 즉 ID3D11Device**를 얻고 싶을 때 

	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,	// 그래픽 카드 사용하겠다는 내용, 그래픽 카드가 없으면 다른 거 선택하게끔 하는 옵션 있어.
		nullptr,
		0,
		nullptr, // feature level 에 대한 배열을 만들어 건내줘야 한다. 몇 DX 버전 지원할것인지, 안채워도 지원 가능한 상위 버전 골라준다.
		0, // 배열크기, nullptr로 했으니 0개
		D3D11_SDK_VERSION, // 11버전에서 세부 버전이 몇번인지 매크로로 정의되어 있어.
		&desc, // 스왑체인 디스크립션을 위에서 만들어서 넣어준다.
		_swapChain.GetAddressOf(), // _device에다가 결국 결과물을 받아주는 거 
		_device.GetAddressOf(),
		nullptr,
		_deviceContext.GetAddressOf()
	);

	//assert(SUCCEEDED(hr)); // 실패하면 크래시 나게 유도할 수 있다.
	CHECK(hr);
}

void Game::CreateRenderTargetView()
{
	HRESULT hr; 

	ComPtr<ID3D11Texture2D> backBuffer = nullptr; 
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf()); // 후면 버퍼를 Texture2D라는 티입으로 반환해 주는 거 
	// Texture는 png같은 건데 앞으로 어떻게 분석할지 모르는 거. RenderTargetView 용도로 사용할 것이라는 걸 명시해 줘야 해. 
	CHECK(hr); 

	_device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf()); 
	CHECK(hr);
	// 흐름: SwapChain에서 후면 버퍼에 해당하는 리소스를 ComPtr<ID3D11Texture2D> backBuffer로 뱉어주고, 
	// 그거를 CreateRenderTargetView를 통해서 backBuffer를 묘사하는 _renderTargetView라는 형태로 만들어 줬고, 
	// GPU와 통신하면서 _renderTargetView를 건내주면 알아먹고, backBuffer에 그림을 그려주는 식으로 동작을 한다.
	// 일종의 스페셜한 포인터
}

void Game::SetViewport()
{
	_viewport.TopLeftX = 0.f; 
	_viewport.TopLeftY = 0.f; 
	_viewport.Width = static_cast<float>(_width); 
	_viewport.Height = static_cast<float>(_height); 
	_viewport.MinDepth = 0.f; 
	_viewport.MaxDepth = 1.f; 
}

void Game::CreateGeometry()
{
	// VertexData - CPU 메모리에 있는 거 
	// 1 3
	// 0 2
	{
		_vertices.resize(4);

		_vertices[0].position = Vec3(-0.5f, -0.5f, 0.f); 
		_vertices[0].uv = Vec2(0.f, 2.f); 
		// _vertices[0].color = Color(1.f, 0.f, 0.f, 1.f); 

		_vertices[1].position = Vec3(-0.5f, 0.5f, 0.f);
		_vertices[1].uv = Vec2(0.f, 0.f);
		// _vertices[1].color = Color(1.f, 0.f, 0.f, 1.f);

		_vertices[2].position = Vec3(0.5f, -0.5f, 0.f);
		_vertices[2].uv = Vec2(2.f, 2.f);
		// _vertices[2].color = Color(1.f, 0.f, 0.f, 1.f);

		_vertices[3].position = Vec3(0.5f, 0.5f, 0.f);
		_vertices[3].uv = Vec2(2.f, 0.f);
		// _vertices[3].color = Color(1.f, 0.f, 0.f, 1.f);
	}

	// VertexBuffer
	{
		D3D11_BUFFER_DESC desc; 
		ZeroMemory(&desc, sizeof(desc)); 
		desc.Usage = D3D11_USAGE_IMMUTABLE; 
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Input assembler에서 건내 주는 vertex buffer를 만들고 있는 것이기 때문
		desc.ByteWidth = (uint32)(sizeof(Vertex) * _vertices.size()); 

		D3D11_SUBRESOURCE_DATA data; 
		ZeroMemory(&data, sizeof(data)); 
		data.pSysMem = _vertices.data(); // &_vertices[0];와 같은 의미

		HRESULT hr = _device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf()); 
		// 설정한 값 desc, data에 의해가지고 GPU 쪽에 버퍼가 만들어 지면서 CPU의 메모리에서 들고 있었던 vertices에 관한 정보가 
		// 복사가 되어 넘어 간다. 그 다음 부터는 GPU의 메모리만 Read Only로 사용하겠구나 예측이 가능하다.  
		CHECK(hr); 
	}

	// Index
	{
		_indices = { 0, 1, 2, 2, 1, 3 }; // 시계방향 골랐으면 시계방향 유지해야 한다. 삼각형 2개 만들어 줬다.
	}

	// IndexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER; // Input assembler에서 건내 주는 index buffer를 만들고 있는 것이기 때문
		desc.ByteWidth = (uint32)(sizeof(uint32) * _indices.size());

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = _indices.data(); // &_indices[0];와 같은 의미

		HRESULT hr = _device->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
		CHECK(hr); 
	}

}

void Game::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, // 12바이트 부터 컬러가 시작 offset
	};

	const int32 count = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	_device->CreateInputLayout(layout, count, _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), _inputLayout.GetAddressOf());  //세번째 인자로 Shader에 대한 정보를 받아주고 있어. 그래서 Shader를 먼저 만들어서 로드를 해야겠구나 알 수 있는 거. 
}

// 셰이더를 로드해서 Blob이라는 걸 만들어준 다음에 그 Blob에 있는 정보들을 이용해서 Vertex shader를 만들어주면 최종적으로 완료가 되면 _vertexShader가 채워진다. 
void Game::CreateVS()
{
	LoadShaderFromFile(L"Default.hlsl", "VS", "vs_5_0", _vsBlob); // _vsBlob이 채워지게 된다. 
	
	HRESULT hr = _device->CreateVertexShader(_vsBlob->GetBufferPointer(),
		_vsBlob->GetBufferSize(), nullptr, _vertexShader.GetAddressOf()); 
	CHECK(hr); 
}

void Game::CreatePS()
{
	LoadShaderFromFile(L"Default.hlsl", "PS", "ps_5_0", _psBlob); // _psBlob이 채워지게 된다. 

	HRESULT hr = _device->CreatePixelShader(_psBlob->GetBufferPointer(),
		_psBlob->GetBufferSize(), nullptr, _pixelShader.GetAddressOf());
	CHECK(hr);
}

void Game::CreateSRV()
{
	DirectX::TexMetadata md; 
	DirectX::ScratchImage img; 
	HRESULT hr = ::LoadFromWICFile(L"chiikawa.png", WIC_FLAGS_NONE, &md, img); 
	CHECK(hr); 

	hr = ::CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView.GetAddressOf()); 
	CHECK(hr); 

	hr = ::LoadFromWICFile(L"hachiware.png", WIC_FLAGS_NONE, &md, img);
	CHECK(hr);

	hr = ::CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourceView2.GetAddressOf());
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
