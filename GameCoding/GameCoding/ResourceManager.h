#pragma once

#include "ResourceBase.h"

class Mesh; 
class Material; 
class Shader; 
class Animation; 
class Texture; 

class ResourceManager
{
public:
	ResourceManager(ComPtr<ID3D11Device> device); 

	void Init(); 

	template<typename T>
	shared_ptr<T> Load(const wstring& key, const wstring& path); // 지금은 구현안함

	template<typename T>
	bool Add(const wstring& key, shared_ptr<T> object); 

	template<typename T>
	shared_ptr<T> Get(const wstring& key);

	// Get<Texture>()
	template<typename T>
	ResourceType GetResourceType(); 

private:
	void CreateDefaultTexture(); 
	void CreateDefaultMesh(); 
	void CreateDefaultShader(); 
	void CreateDefaultMaterial(); 
	void CreateDefaultAnimation(); 

private:
	ComPtr<ID3D11Device> _device; 

	using KeyObjMap = map<wstring/*key*/, shared_ptr<ResourceBase>>; 

	// [ map map map map map ]
	array<KeyObjMap, RESOURCE_TYPE_COUNT> _resources; 
};

template<typename T>
inline shared_ptr<T> ResourceManager::Load(const wstring& key, const wstring& path)
{
	auto objectType = GetResourceType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	shared_ptr<T> object = make_shared<T>();
	object->Load(path);
	keyObjMap[key] = object;

	return object;
}

template<typename T>
inline bool ResourceManager::Add(const wstring& key, shared_ptr<T> object)
{
	ResourceType resourceType = GetResourceType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(resourceType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return false; 

	keyObjMap[key] = object;

	return true; 
}

template<typename T>
inline shared_ptr<T> ResourceManager::Get(const wstring& key)
{
	ResourceType resourceType = GetResourceType<T>();
	KeyObjMap& keyObjMap = _resources[static_cast<uint8>(resourceType)]; 

	auto findIt = keyObjMap.find(key); 
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second); 

	return nullptr; 
}

template<typename T>
inline ResourceType ResourceManager::GetResourceType()
{
	if (std::is_same_v<T, Texture>) // 어떤 template 타입이 Texture타입과 일치하는지, 컴파일 타임에 실행되기 원하면 constexpr를 붙인다. 지금은 런타임 실행
		return ResourceType::Texture; 

	if (std::is_same_v<T, Mesh>) 
		return ResourceType::Mesh;

	if (std::is_same_v<T, Shader>)
		return ResourceType::Shader;

	if (std::is_same_v<T, Material>)
		return ResourceType::Material;

	if (std::is_same_v<T, Animation>)
		return ResourceType::Animation;

	assert(false); 
	return ResourceType::None;
}
