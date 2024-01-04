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
	void CreateRasterizerState();
	void CreateSamplerState();
	void CreateBlendState();

	void LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob); 

private:
	HWND _hwnd;

	shared_ptr<Graphics> _graphics; 

private: 
	// 전체
	// 클래스
	// 객체

	// Geometry
	shared_ptr<Geometry<VertexTextureData>> _geometry;
	shared_ptr<VertexBuffer> _vertexBuffer; 
	shared_ptr<IndexBuffer> _indexBuffer; 
	shared_ptr<InputLayout> _inputLayout; 

	// VS
	shared_ptr<VertexShader> _vertexShader;
	
	// RAS
	ComPtr<ID3D11RasterizerState> _rasterizerState = nullptr; 

	// PS
	shared_ptr<PixelShader> _pixelShader; 

	// SRV
	shared_ptr<Texture> _texture1;

	ComPtr<ID3D11SamplerState> _samplerState = nullptr; 
	ComPtr<ID3D11BlendState> _blendState = nullptr; 

	// [CPU <-> RAM] [GPU <-> VRAM]
private:
	TransformData _transformData;
	shared_ptr<ConstantBuffer<TransformData>> _constantBuffer; 
//	ComPtr<ID3D11Buffer> _constantBuffer;

	Vec3 _localPosition = { 0.f, 0.f, 0.f }; 
	Vec3 _localRotation = { 0.f, 0.f, 0.f };
	Vec3 _localScale = { 1.f, 1.f, 1.f }; 
};