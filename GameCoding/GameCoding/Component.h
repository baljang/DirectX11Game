#pragma once

class GameObject; 

class Component
{
public:
	Component(); 
	~Component(); 

	virtual void Init() abstract; 
	virtual void Update() abstract; 

	// 유니티에선 gameobject로 되어 있다. 
	shared_ptr<GameObject> GetGameObject() { return _owner.lock(); } 

protected: 
	weak_ptr<GameObject> _owner;
};

