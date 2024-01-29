#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "ResourceManager.h"
#include "Game.h"
#include "Mesh.h"
#include "Animator.h"

SceneManager::SceneManager(shared_ptr<Graphics> graphics)
	: _graphics(graphics)
{

}

void SceneManager::Init()
{
	if (_activeScene == nullptr)
		return; 

	_activeScene->Awake(); 
	_activeScene->Start(); 
	// 간단하게 묘사하기 위해 이렇게만 해본다.실제론 복잡한 순서

}

void SceneManager::Update()
{
	if (_activeScene == nullptr)
		return; 

	_activeScene->Update(); 
	_activeScene->LateUpdate(); 

	_activeScene->FixedUpdate(); 
	// 원래대로라면 흐름에 맞게 이루어져 있다. FixedUpdate면 고정 프레임으로 실행되는 무엇인가 등등
}

void SceneManager::LoadScene(wstring sceneName)
{
	// Resource

	_activeScene = LoadTestScene(); 
	Init(); 
}

shared_ptr<Scene> SceneManager::LoadTestScene()
{
	shared_ptr<Scene> scene = make_shared<Scene>(); 

	// Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>(_graphics->GetDevice(), _graphics->GetDeviceContext());
		{
			camera->GetOrAddTransform();
			camera->AddComponent(make_shared<Camera>());
			scene->AddGameObject(camera); 
		}
	}

	// Monster
	{
		shared_ptr<GameObject> monster = make_shared<GameObject>(_graphics->GetDevice(), _graphics->GetDeviceContext());
		monster->GetOrAddTransform()->SetPosition(Vec3{ 1.f, 1.f, 0.f });
		
		{
			monster->GetOrAddTransform();
			auto meshRenderer = make_shared<MeshRenderer>(_graphics->GetDevice(), _graphics->GetDeviceContext());
			monster->AddComponent(meshRenderer);

			auto material = RESOURCES->Get<Material>(L"Default"); 
			meshRenderer->SetMaterial(material); 

			auto mesh = RESOURCES->Get<Mesh>(L"Rectangle"); 
			meshRenderer->SetMesh(mesh); 
		}		
		{
			auto animator = make_shared<Animator>(); 
			monster->AddComponent(animator); 
			auto anim = RESOURCES->Get<Animation>(L"SnakeAnim"); 
			animator->SetAnimation(anim); 
		}
		scene->AddGameObject(monster);
	}	
	
	// Monster
	{
		shared_ptr<GameObject> monster = make_shared<GameObject>(_graphics->GetDevice(), _graphics->GetDeviceContext());
		{
			monster->GetOrAddTransform();
			auto meshRenderer = make_shared<MeshRenderer>(_graphics->GetDevice(), _graphics->GetDeviceContext());
			monster->AddComponent(meshRenderer);

			auto material = RESOURCES->Get<Material>(L"Default"); 
			meshRenderer->SetMaterial(material); 

			auto mesh = RESOURCES->Get<Mesh>(L"Rectangle"); 
			meshRenderer->SetMesh(mesh); 
		}		
		{
			auto animator = make_shared<Animator>(); 
			monster->AddComponent(animator); 
			auto anim = RESOURCES->Get<Animation>(L"SnakeAnim"); 
			animator->SetAnimation(anim); 
		}
		scene->AddGameObject(monster);
	}

	return scene; 
}
