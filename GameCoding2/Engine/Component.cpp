#include "pch.h"
#include "Component.h"
#include "GameObject.h"

Component::Component(ComponentType type) : _type(type)
{
}

Component::~Component()
{
}

// gameObject
shared_ptr<GameObject> Component::GetGameObject()
{
    return _gameObject.lock();
}

// trasform
shared_ptr<Transform> Component::GetTransform()
{
    return _gameObject.lock()->GetTransform();
}
