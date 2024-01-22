#pragma once

enum class ResourceType : uint8
{
	None = -1,
	Mesh,
	Shader, 
	Texture,
	Material,
	Animation, 

	End
};

enum
{
	RESOURCE_TYPE_COUNT = static_cast<uint8>(ResourceType::End)
};

class ResourceBase : public enable_shared_from_this<ResourceBase>
{
public:
	ResourceBase(ResourceType type); 
	virtual ~ResourceBase(); 

	ResourceType GetType() { return _type; }

	void SetName(const wstring& name) { _name = name; }
	const wstring& GetName() { return _name; }
	uint32 GetID() { return _id; }

protected:
	virtual void Load(const wstring& path) { } 
	virtual void Save(const wstring& path) { }
// 저장하는 포멧은 리소스마다 다르기 때문에 오버로딩해서 정하라고 넘겨주는 거  

protected:
	ResourceType _type = ResourceType::None;
	wstring _name; 

	wstring _path; 
	uint32 _id = 0; 
};

