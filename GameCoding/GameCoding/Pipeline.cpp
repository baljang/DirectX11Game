#include "pch.h"
#include "Pipeline.h"

Pipeline::Pipeline(ComPtr<ID3D11DeviceContext> deviceContext)
	: _deviceContext(deviceContext)
{
}

Pipeline::~Pipeline()
{
}

void Pipeline::UpdatePipeline(PipelineInfo info)
{
	// Game::Render의 일부를 꺼내 온다. 일단 다 가져 온 다음에 걸러냈다.

	// IA	
	// vertexBuffer, indexBuffer는 나중에
	_deviceContext->IASetInputLayout(info.inputLayout->GetComPtr().Get()); // 어떻게 생겨먹은 애인지 묘사해줘야 해
	_deviceContext->IASetPrimitiveTopology(info.topology); // 삼각형으로 인지를 해달라 부탁 

	// VS
	// 왜  ConstantBuffer를 세팅하는 부분은 왜 info에 포함되지 않는걸까? ->상수 버퍼는 사용할지 말지가 명확하지 않기 때문이다.
	// 물체 마다 달라질 수 있는 부분은 따로 별도의 함수를 빼줄 것이고, 여기서는 일반적인 공용 부분만 묶어준다. 그래서 여기서 뺐다.
	if(info.vertexShader)
		_deviceContext->VSSetShader(info.vertexShader->GetComPtr().Get(), nullptr, 0);

	// RS
	if(info.rasterizerState) // 이것도 사실 옵션이긴 하다. 있으면 세팅해준다.
		_deviceContext->RSSetState(info.rasterizerState->GetComPtr().Get());

	// PS
	// shaderResource, sampler도 사용할지 말지 모르니 건너 띈다.
	if(info.pixelShader)
		_deviceContext->PSSetShader(info.pixelShader->GetComPtr().Get(), nullptr, 0);

	// OM
	if(info.blendState)
		_deviceContext->OMSetBlendState(info.blendState->GetComPtr().Get(), info.blendState->GetBlendFactor(), info.blendState->GetSampleMask());
}

void Pipeline::SetVertexBuffer(shared_ptr<VertexBuffer> buffer)
{
	uint32 stride = buffer->GetStride(); 
	uint32 offset = buffer->GetOffset(); 

	// Game::Render를 참고해서 만들어 보면
	_deviceContext->IASetVertexBuffers(0, 1, buffer->GetComPtr().GetAddressOf(), &stride, &offset);

}

void Pipeline::SetIndexBuffer(shared_ptr<IndexBuffer> buffer)
{
	_deviceContext->IASetIndexBuffer(buffer->GetComPtr().Get(), DXGI_FORMAT_R32_UINT, 0);
}

void Pipeline::SetTexture(uint32 slot, uint32 scope, shared_ptr<Texture> texture)
{
	if(scope & SS_VertexShader)
		_deviceContext->VSSetShaderResources(slot, 1, texture->GetComPtr().GetAddressOf());
	
	if (scope & SS_PixelShader)
		_deviceContext->PSSetShaderResources(slot, 1, texture->GetComPtr().GetAddressOf());
}

void Pipeline::SetSamplerState(uint32 slot, uint32 scope, shared_ptr<SamplerState> samplerState)
{
	if (scope & SS_VertexShader)
		_deviceContext->VSSetSamplers(0, 1, samplerState->GetComPtr().GetAddressOf());

	if (scope & SS_PixelShader)
		_deviceContext->PSSetSamplers(0, 1, samplerState->GetComPtr().GetAddressOf());
}

void Pipeline::Draw(uint32 vertexCount, uint32 startVertexLocation)
{		
	_deviceContext->Draw(vertexCount, startVertexLocation);
}

void Pipeline::DrawIndexed(uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation)
{
	_deviceContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}
