#pragma once
#include "InstancingBuffer.h"

class GameObject; 

class InstancingManager
{
	DECLARE_SINGLE(InstancingManager); 

public: 
	void Render(vector<shared_ptr<GameObject>>& gameObjects);
	void ClearData(); // 이전 프레임에 등록되어 있는 데이터는 날려주는거 

private:
	// MeshRenderer를 들고 있는 object만 보여주는 함수
	void RenderMeshRenderer(vector<shared_ptr<GameObject>>& gmaeObjects);
	void RenderModelRenderer(vector<shared_ptr<GameObject>>& gmaeObjects);
	void RenderAnimRenderer(vector<shared_ptr<GameObject>>& gmaeObjects);

private:
	void AddData(InstanceID instanceId, InstancingData& data);

private: 
	map<InstanceID, shared_ptr<InstancingBuffer>> _buffers;
};

