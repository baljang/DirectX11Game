#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "BaseCollider.h"
#include "Camera.h"

void Scene::Start()
{
	// Start, Updtae, LaterUpdate를 하는 도중에 Add, Remove 되는걸 방지하기 위해
	unordered_set<shared_ptr<GameObject>> objects = _objects;
	
	for (shared_ptr<GameObject> object : objects)
	{
		object->Start(); 
	}
}

void Scene::Update()
{
	unordered_set<shared_ptr<GameObject>> objects = _objects;

	for (shared_ptr<GameObject> object : objects)
	{
		object->Update();
	}

	// INSTANCING
	vector<shared_ptr<GameObject>> temp;
	temp.insert(temp.end(), objects.begin(), objects.end());
	INSTANCING->Render(temp);
}

void Scene::LateUpdate()
{
	unordered_set<shared_ptr<GameObject>> objects = _objects;

	for (shared_ptr<GameObject> object : objects)
	{
		object->LateUpdate();
	}

	CheckCollision(); 
}

void Scene::Add(shared_ptr<GameObject> object)
{
	_objects.insert(object); 

	if (object->GetCamera() != nullptr)
	{
		_cameras.insert(object); 
	}

	if (object->GetLight() != nullptr)
	{
		_lights.insert(object); 
	}
}

void Scene::Remove(shared_ptr<GameObject> object)
{
	_objects.erase(object); 

	_cameras.erase(object); 

	_lights.erase(object); 
}

std::shared_ptr<class GameObject> Scene::Pick(int32 screenX, int32 screenY)
{
	shared_ptr<Camera> camera = GetCamera()->GetCamera(); // Get카메라 오브젝트->Get카메라 컴포넌트

	float width = GRAPHICS->GetViewport().GetWidth(); 
	float height = GRAPHICS->GetViewport().GetHeight(); 

	Matrix projectionMatrix = camera->GetProjectionMatrix(); 

	float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0); // Scrren좌표를 View 좌표로 치환, 공식은 예전 문서 참조
	float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1); 

	Matrix viewMatrix = camera->GetViewMatrix(); 
	Matrix viewMatrixInv = viewMatrix.Invert(); 

	const auto& gameObjects = GetObjects(); // 모든 물체들 중에서 피격이 되는게 있는지 확인 하기 위해

	float minDistance = FLT_MAX; // 큰 값으로 시작
	shared_ptr<GameObject> picked;  // 피킹된 오브젝트 

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetCollider() == nullptr)
			continue; 

		// ViewSpace에서 Ray 정의
		Vec4 rayOrigin = Vec4(0.f, 0.f, 0.f, 1.f); // ViewSpace라는게 카메라의 원점을 기준으로 하는 좌표계이다 보니 0,0,0이 카메라의 위치가 된다.
		Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.f); // 클릭했던 좌표를 이용해서 시작점과 끝점을 구해주고 있는 상황이다. 

		// WorldSpace에서의 Ray 정의
		// World 기준으로 되돌린다
		Vec3 worldRayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);  // 위치까지
		Vec3 worldRayDir = XMVector3TransformNormal(rayDir, viewMatrixInv); // 방향만
		worldRayDir.Normalize(); 

		// WorldSpace에서 연산
		Ray ray = Ray(worldRayOrigin, worldRayDir); 
		// World에서 했지만, View 스페이스에서 해도 말이 된다. 기준이 되는 세상만 고정을 해서 만들어 주면 된다.

		float distance = 0.f; 
		if (gameObject->GetCollider()->Intersects(ray, OUT distance) == false)
			continue; 

		if (distance < minDistance)
		{
			minDistance = distance; 
			picked = gameObject; 
		}
	}

	return picked; 
}

void Scene::CheckCollision()
{
	vector<shared_ptr<BaseCollider>> colliders; 

	for (shared_ptr<GameObject> object : _objects)
	{
		if (object->GetCollider() == nullptr)
			continue; 

		colliders.push_back(object->GetCollider()); 
	}

	// BruteForce
	for (int32 i = 0; i < colliders.size(); i++)
	{
		for (int32 j = i + 1; j < colliders.size(); j++)
		{
			shared_ptr<BaseCollider>& other = colliders[j]; 
			if (colliders[i]->Intersects(other))
			{
				int a = 3; 
			}
		}
	}
}
