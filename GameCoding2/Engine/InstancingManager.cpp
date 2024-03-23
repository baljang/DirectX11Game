#include "pch.h"
#include "InstancingManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"

void InstancingManager::Render(vector<shared_ptr<GameObject>>& gameObjects)
{

	ClearData(); 

	RenderMeshRenderer(gameObjects); // 매 프레임마다 모으는 작업
	RenderModelRenderer(gameObjects); 
	RenderAnimRenderer(gameObjects);
}

// map을 날리는게 아니라 InstancingBuffer에 있는 vector<InstancingData>	 _data; 이걸 날린다는 얘기다
void InstancingManager::ClearData() 
{
	// 싹 다 밀어준다.
	for (auto& pair : _buffers)
	{
		shared_ptr<InstancingBuffer>& buffer = pair.second; // shared_ptr의 레퍼런스 카운트를 1 늘리는게 아토믹해서 부담이 크기 때문에 일시적으로 활용할 때는 복사하지 않고 참조값을 붙여서 쓴다. 
		buffer->ClearData(); 
	}
}

void InstancingManager::RenderMeshRenderer(vector<shared_ptr<GameObject>>& gameObjects)
{
	map<InstanceID, vector<shared_ptr<GameObject>>> cache; 

	// 분류 단계
	for (shared_ptr<GameObject>& gameObject : gameObjects) //  &를 써서 레퍼런스 카운터를 안가지고 와서 쓰면 문제가 되는 부분이 없을까? 다른 데서 레퍼런스 카운트를 줄일 때 날아갈 수 있다. 멀티 스레드에서는 위험하지만 싱글 스레드에서는 문제 없다.
	{
		if (gameObject->GetMeshRenderer() == nullptr)
			continue; 

		const InstanceID instanceId = gameObject->GetMeshRenderer()->GetInstanceID(); 
		cache[instanceId].push_back(gameObject); // 같은 애들끼리 분리수거를 해주는 것이다. 
	}

	for (auto& pair : cache)
	{
		const vector<shared_ptr<GameObject>>& vec = pair.second; 

		//if (vec.size() == 1)
		//{
		//    하나만 있을 때는 옛날 버전으로 하면 된다. 
		//}
		//else 무조건 인스턴싱을 적용하는 버전으로 만든다.
		{
			const InstanceID instanceId = pair.first; 

			for (int32 i = 0; i < vec.size(); i++)
			{
				const shared_ptr<GameObject>& gameObject = vec[i]; 

				InstancingData data;
				data.world = gameObject->GetTransform()->GetWorldMatrix(); 

				AddData(instanceId, data); // InstanceId에 따라서 InstancingBuffer를 찾아준 다음에 버퍼에다가 데이터를 넣어주는 함수
			}	

			// 막타
			// 대표로 할 애를 정하면 된다. 
			shared_ptr<InstancingBuffer>& buffer = _buffers[instanceId]; 
			vec[0]->GetMeshRenderer()->RenderInstancing(buffer); 
		}
	}
}

void InstancingManager::RenderModelRenderer(vector<shared_ptr<GameObject>>& gmaeObjects)
{
	map<InstanceID, vector<shared_ptr<GameObject>>> cache;

	// 분류 단계
	for (shared_ptr<GameObject>& gameObject : gmaeObjects) //  &를 써서 레퍼런스 카운터를 안가지고 와서 쓰면 문제가 되는 부분이 없을까? 다른 데서 레퍼런스 카운트를 줄일 때 날아갈 수 있다. 멀티 스레드에서는 위험하지만 싱글 스레드에서는 문제 없다.
	{
		if (gameObject->GetModelRenderer() == nullptr)
			continue;

		const InstanceID instanceId = gameObject->GetModelRenderer()->GetInstanceID();
		cache[instanceId].push_back(gameObject); // 같은 애들끼리 분리수거를 해주는 것이다. 
	}

	for (auto& pair : cache)
	{
		const vector<shared_ptr<GameObject>>& vec = pair.second;

		//if (vec.size() == 1)
		//{
		//    하나만 있을 때는 옛날 버전으로 하면 된다. 
		//}
		//else 무조건 인스턴싱을 적용하는 버전으로 만든다.
		{
			const InstanceID instanceId = pair.first;

			for (int32 i = 0; i < vec.size(); i++)
			{
				const shared_ptr<GameObject>& gameObject = vec[i];

				InstancingData data;
				data.world = gameObject->GetTransform()->GetWorldMatrix();

				AddData(instanceId, data); // InstanceId에 따라서 InstancingBuffer를 찾아준 다음에 버퍼에다가 데이터를 넣어주는 함수
			}

			// 막타
			// 대표로 할 애를 정하면 된다. 
			shared_ptr<InstancingBuffer>& buffer = _buffers[instanceId];
			vec[0]->GetModelRenderer()->RenderInstancing(buffer);
		}
	}

}

void InstancingManager::RenderAnimRenderer(vector<shared_ptr<GameObject>>& gameObjects)
{
	map<InstanceID, vector<shared_ptr<GameObject>>> cache;

	// 분류 단계
	for (shared_ptr<GameObject>& gameObject : gameObjects) //  &를 써서 레퍼런스 카운터를 안가지고 와서 쓰면 문제가 되는 부분이 없을까? 다른 데서 레퍼런스 카운트를 줄일 때 날아갈 수 있다. 멀티 스레드에서는 위험하지만 싱글 스레드에서는 문제 없다.
	{
		if (gameObject->GetModelAnimator() == nullptr)
			continue;

		const InstanceID instanceId = gameObject->GetModelAnimator()->GetInstanceID();
		cache[instanceId].push_back(gameObject); // 같은 애들끼리 분리수거를 해주는 것이다. 
	}

	for (auto& pair : cache)
	{
		const vector<shared_ptr<GameObject>>& vec = pair.second;

		//if (vec.size() == 1)
		//{
		//	vec[0]->GetModelAnimator()->RenderSingle(); 
		//    하나만 있을 때는 옛날 버전으로 하면 된다. 
		//}
		//else 무조건 인스턴싱을 적용하는 버전으로 만든다.
		{
			shared_ptr<InstancedTweenDesc> tweenDesc = make_shared<InstancedTweenDesc>();


			const InstanceID instanceId = pair.first;

			for (int32 i = 0; i < vec.size(); i++)
			{
				const shared_ptr<GameObject>& gameObject = vec[i];

				InstancingData data;
				data.world = gameObject->GetTransform()->GetWorldMatrix();

				AddData(instanceId, data); // InstanceId에 따라서 InstancingBuffer를 찾아준 다음에 버퍼에다가 데이터를 넣어주는 함수
			
				// INSTANCING
				gameObject->GetModelAnimator()->UpdateTweenData(); 
				tweenDesc->tweens[i] = gameObject->GetModelAnimator()->GetTweenDesc();
			}

			vec[0]->GetModelAnimator()->GetShader()->PushTweenData(*tweenDesc.get());

			// 막타
			// 대표로 할 애를 정하면 된다. 
			shared_ptr<InstancingBuffer>& buffer = _buffers[instanceId];
			vec[0]->GetModelAnimator()->RenderInstancing(buffer);
		}
	}
}

void InstancingManager::AddData(InstanceID instanceId, InstancingData& data)
{
	if (_buffers.find(instanceId) == _buffers.end())
		_buffers[instanceId] = make_shared<InstancingBuffer>(); 

	_buffers[instanceId]->AddData(data); 
}
