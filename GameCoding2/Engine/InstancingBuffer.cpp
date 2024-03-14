#include "pch.h"
#include "InstancingBuffer.h"

InstancingBuffer::InstancingBuffer()
{
	CreateBuffer(MAX_MESH_INSTANCE); 
}

InstancingBuffer::~InstancingBuffer()
{
}

void InstancingBuffer::ClearData()
{
	_data.clear(); 
}

void InstancingBuffer::AddData(InstancingData& data)
{
	_data.push_back(data); 
}

void InstancingBuffer::PushData()
{
	const uint32 dataCount = GetCount(); 
	if (dataCount > _maxCount)
		CreateBuffer(dataCount); 

	D3D11_MAPPED_SUBRESOURCE subResource; 

	DC->Map(_instanceBuffer->GetComPtr().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource); 
	{
		::memcpy(subResource.pData, _data.data(), sizeof(InstancingData) * dataCount); 
	}
	DC->Unmap(_instanceBuffer->GetComPtr().Get(), 0); 

	_instanceBuffer->PushData(); // GPU에 던져줘야 한다.
}

void InstancingBuffer::CreateBuffer(uint32 maxCount)
{
	_maxCount = maxCount; 
	_instanceBuffer = make_shared<VertexBuffer>(); 

	vector<InstancingData> temp(maxCount); 
	_instanceBuffer->Create(temp, /*slot*/1, /*cpuWrite*/true); // cpuWrite가 true여야 매 프래임마다 _instanceBuffer를 수정할 수 있다. 
	// 이제 얼마든지 데이터를 갱신할 수 있는데 갱신하기 위한 정보를 pushData에서 작업을 해준다. 
}
