#pragma once

class VertexBuffer; 

struct InstancingData
{
	Matrix world; 
};

#define MAX_MESH_INSTANCE 500

class InstancingBuffer
{
public: 
	InstancingBuffer(); 
	~InstancingBuffer(); 

public:
	void ClearData(); // 매 프레임마다 물체들이 갱신 되기 때문에 싹 비워준 다음에 데이터를 채워준다. 그 데이터를 누군가가 대표해서 쉐이더 쪽으로 밀어넣는 식으로 작업을 한다.
	void AddData(InstancingData& data); // 매 프레임마다 정보를 밀어 넣기 위한 함수
	void PushData(); 

public:
	uint32 GetCount() { return static_cast<uint32>(_data.size()); }
	shared_ptr<VertexBuffer> GetBuffer() { return _instanceBuffer; }

private:
	void CreateBuffer(uint32 maxCount = MAX_MESH_INSTANCE); // 벡터가 늘어났다 줄어들었다 하는 것처럼 늘어날 수 있으니까

private:
	// uint64						_instanceId = 0; 
	shared_ptr<VertexBuffer>	_instanceBuffer; // shader에 pushData로 넣어줄 예정
	uint32						_maxCount = 0; 
	vector<InstancingData>		_data; 
};

