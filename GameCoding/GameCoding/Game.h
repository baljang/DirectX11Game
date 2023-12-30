#pragma once
class Game
{
public:
	Game();
	~Game();

public:
	void Init(HWND hwnd);
	void Update();
	void Render();

private: 

private: 
	void CreateGeometry(); 
	void CreateInputLayout();

	void CreateVS(); 
	void CreatePS(); 

	void CreateRasterizerState();
	void CreateSamplerState();
	void CreateBlendState();
	void CreateSRV(); 

	void CreateContantBuffer(); 

	void LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob); 

private:
	HWND _hwnd;
//	uint32 _width = 0;
//	uint32 _height = 0;

	Graphics* _graphics; 

private: 
	// Geometry
	vector<Vertex> _vertices; 
	ComPtr<ID3D11Buffer> _vertexBuffer = nullptr; 
	vector<uint32> _indices; // uint16 중 선택사항인데 일단은 4바이트로 가본다.인덱스 목록 CPU에 들고 있음. 
	ComPtr<ID3D11Buffer> _indexBuffer = nullptr; 
	ComPtr<ID3D11InputLayout> _inputLayout = nullptr;

	// VS
	ComPtr<ID3D11VertexShader> _vertexShader = nullptr; 
	ComPtr<ID3DBlob> _vsBlob = nullptr; 
	
	// RAS
	ComPtr<ID3D11RasterizerState> _rasterizerState = nullptr; 

	// PS
	ComPtr<ID3D11PixelShader> _pixelShader = nullptr; 
	ComPtr<ID3DBlob> _psBlob = nullptr; 

	// SRV
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView = nullptr; 
	ComPtr<ID3D11ShaderResourceView> _shaderResourceView2 = nullptr; 

	ComPtr<ID3D11SamplerState> _samplerState = nullptr; 
	ComPtr<ID3D11BlendState> _blendState = nullptr; 

	// [CPU <-> RAM] [GPU <-> VRAM]
private:
	TransformData _transformData;
	ComPtr<ID3D11Buffer> _constantBuffer;

	Vec3 _localPosition = { 0.f, 0.f, 0.f }; 
	Vec3 _localRotation = { 0.f, 0.f, 0.f };
	Vec3 _localScale = { 1.f, 1.f, 1.f }; 
};