#pragma once

class VertexBuffer
{
public:
	VertexBuffer(ComPtr<ID3D11Device> device); 
	~VertexBuffer(); 

	ComPtr<ID3D11Buffer> GetComPtr() { return _vertexBuffer; }
	uint32 GetStride() { return _stride; }
	uint32 GetOffset() { return _offset; }
	uint32 GetCount() { return _count; }

	template<typename T>
	void Create(const vector<T>& vertices)
	{
		_stride = sizeof(T); 
		_count = static_cast<uint32>(vertices.size()); 

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Input assembler에서 건내 주는 vertex buffer를 만들고 있는 것이기 때문
		desc.ByteWidth = (uint32)(_stride * _count);

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = vertices.data(); // &_vertices[0];와 같은 의미

		HRESULT hr = _device->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
		// 설정한 값 desc, data에 의해가지고 GPU 쪽에 버퍼가 만들어 지면서 CPU의 메모리에서 들고 있었던 vertices에 관한 정보가 
		// 복사가 되어 넘어 간다. 그 다음 부터는 GPU의 메모리만 Read Only로 사용하겠구나 예측이 가능하다.  
		CHECK(hr);
	}

private:
	ComPtr<ID3D11Device> _device; 
	ComPtr<ID3D11Buffer> _vertexBuffer = nullptr;

	uint32 _stride = 0; 
	uint32 _offset = 0; 
	uint32 _count = 0; 
};

