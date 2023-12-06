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

}

void Game::Update()
{
}

void Game::Render()
{
	RenderBegin(); 

	// TODO

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
