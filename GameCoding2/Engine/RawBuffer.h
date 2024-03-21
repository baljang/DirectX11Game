#pragma once
class RawBuffer
{
public: 
	RawBuffer(void* inputData, uint32 inputByte, uint32 outputByte); 
	~RawBuffer(); 

public:
	void CreateBuffer(); 
	void CopyToInput(void* data); // CPU 쪽 메모리에서 데이터를 만들어줘서 그것을 건내주세요하면 _input Buffer에 저장이 되어서 건내주는 형태
	void CopyFromOutput(void* data); // _result Buffer에 들어가 있는 것을 다시 긁어 오는 형태로 작업

public:
	ComPtr<ID3D11ShaderResourceView> GetSRV() { return _srv; }
	ComPtr<ID3D11UnorderedAccessView> GetUAV() { return _uav; }

private:
	void CreateInput(); 
	void CreateSRV(); 
	void CreateOutput(); 
	void CreateUAV(); 
	void CreateResult();

private:
	ComPtr<ID3D11Buffer> _input; 
	ComPtr<ID3D11ShaderResourceView> _srv; 
	ComPtr<ID3D11Buffer> _output; 
	ComPtr<ID3D11UnorderedAccessView> _uav; 
	ComPtr<ID3D11Buffer> _result; 

private: 
	void* _inputData; // 입력받을 데이터
	uint32 _inputByte = 0; // 크기 
	uint32 _outputByte = 0; // 리턴해야 할 데이터 크기	 
};

