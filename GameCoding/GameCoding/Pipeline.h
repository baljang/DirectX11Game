#pragma once

// 기본적으로 모든애들이 공통적으로 사용하는 정보를 빼준다.이걸 세팅해서 포탄을 갈아 끼우는 것 처럼 작업을 해준다.
struct PipelineInfo
{
	// Game.h에서 어떤 애들은 물체마다 고유하게 변하는 애들이 있고, 
	// shader 같이 물체마다 공유해서 사용하는 애들도 있다.
	// 그런 부분들을 분리해서 관리해 본다. 

	// 이런 옵션들을 이용해 렌더링 파이프 라인을 고칠 수 있다고 연결을 해 놓는 거 
	shared_ptr<InputLayout> inputLayout; 
	shared_ptr<VertexShader> vertexShader; // 상용 엔진의 경우 공용 shader 하나를 돌려 쓰고, 일부 특정 물체는 전용 shader를 쓰는 경우가 많다. 
	shared_ptr<PixelShader> pixelShader;
	shared_ptr<RasterizerState> rasterizerState; 
	shared_ptr<BlendState> blendState; 
	D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; 
};
// 이 정보를 토대로 포탄을 갈아 끼는 거다.

class Pipeline
{
public:
	Pipeline(ComPtr<ID3D11DeviceContext> deviceContext); 
	~Pipeline(); 
	
	void UpdatePipeline(PipelineInfo info); // 정보를 갈아 끼운다. 

	// 버텍스 버퍼, 인덱스 버퍼를 채워주는 거 
	void SetVertexBuffer(shared_ptr<VertexBuffer> buffer); 
	void SetIndexBuffer(shared_ptr<IndexBuffer> buffer); 

	template<typename T>
	void SetConstantBuffer(uint32 slot, uint32 scope, shared_ptr<ConstantBuffer<T>> buffer)
	{
		if(scope & SS_VertexShader)
			_deviceContext->VSSetConstantBuffers(slot, 1, buffer->GetComPtr().GetAddressOf());
		if (scope & SS_PixelShader)
			_deviceContext->PSSetConstantBuffers(slot, 1, buffer->GetComPtr().GetAddressOf());
	}
	
	// slot은 chiikawa일지 hachiware일지
	// 텍스쳐를 받아서 세팅해 주는 부분까지 연동
	void SetTexture(uint32 slot, uint32 scope, shared_ptr<Texture> texture);
	void SetSamplerState(uint32 slot, uint32 scope, shared_ptr<SamplerState> samplerState); 

	void Draw(uint32 vertexCount, uint32 startVertexLocation); 
	void DrawIndexed(uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation); 


private:
	ComPtr<ID3D11DeviceContext> _deviceContext; 
};

