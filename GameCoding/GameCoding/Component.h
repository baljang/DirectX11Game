#pragma once

class GameObject; 
class Transform;

enum class ComponentType : uint8
{
	Transform, 
	MeshRenderer, 
	Camera, 
	Animator, 
	// ...
	Script, 

	End, 
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<uint8>(ComponentType::End) - 1 // 1을 빼서 4개가 고정적인 애라고 표현 
};

class Component
{
public:
	Component(ComponentType type); 
	~Component(); 

	virtual void Awake() {}
	virtual void Start() {}
	virtual void Update() {}
	virtual void LateUpdate() {}
	virtual void FixedUpdate() {}

public:
	ComponentType GetType() { return _type; }

	// 유니티에선 gameobject로 되어 있다. 
	shared_ptr<GameObject> GetGameObject();

	void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }

	shared_ptr<Transform> GetTransform();

protected: 
	weak_ptr<GameObject> _gameObject;
	ComponentType _type; 

private:
	friend class GameObject; 
};

