#pragma once

enum ShaderScope
{
	SS_None = 0,
	SS_VertexShader = (1 << 0), // 1
	SS_PixelShader = (1 << 1), // 비트 연산 2, 비트플래그 용도로 활용할 거란 힌트를 주는 거 
	SS_Both = SS_VertexShader | SS_PixelShader
};

class Shader
{
public: 
	Shader(ComPtr<ID3D11Device> device); 
	virtual ~Shader();

	virtual void Create(const wstring& path, const string& name, const string& version) abstract; 

	ComPtr<ID3DBlob> GetBlob() { return _blob; }

protected:
	void LoadShaderFromFile(const wstring& path, const string& name, const string& version);

protected:
	wstring _path; 
	string _name; 
	ComPtr<ID3D11Device> _device; 
	ComPtr<ID3DBlob> _blob = nullptr;
};

class VertexShader : public Shader
{
	using Super = Shader; 
public: 
	VertexShader(ComPtr<ID3D11Device> device); 
	~VertexShader(); 

	ComPtr<ID3D11VertexShader> GetComPtr() { return _vertexShader; }

	virtual void Create(const wstring& path, const string& name, const string& version) override;

protected: 
	ComPtr<ID3D11VertexShader> _vertexShader = nullptr;
};

class PixelShader : public Shader
{
	using Super = Shader;
public:
	PixelShader(ComPtr<ID3D11Device> device);
	~PixelShader();

	ComPtr<ID3D11PixelShader> GetComPtr() { return _pixelShader; }

	virtual void Create(const wstring& path, const string& name, const string& version) override;

protected:
	ComPtr<ID3D11PixelShader> _pixelShader = nullptr;
};