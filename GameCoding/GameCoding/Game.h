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

	shared_ptr<Graphics> _graphics; 

private: 
	// Geometry
	shared_ptr<Geometry<VertexTextureData>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer; 
	shared_ptr<IndexBuffer> _indexBuffer; 
	shared_ptr<InputLayout> _inputLayout; 

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