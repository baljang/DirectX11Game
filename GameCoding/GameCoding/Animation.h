#pragma once
#include "ResourceBase.h"

struct Keyframe
{
	// 어디부터 어디까지 그릴 것인지
	Vec2 offset = Vec2{ 0.f, 0.f }; 
	Vec2 size = Vec2{ 0.f, 0.f }; 

	// 몇 초 통안 그릴 것인지
	float time = 0.f; 
};

class Texture;  

class Animation : public ResourceBase
{
	using Super = ResourceBase;
public:
	Animation(); 
	virtual ~Animation(); 

	virtual void Load(const wstring& path) override; 
	virtual void Save(const wstring& path) override; 

	void SetLoop(bool loop) { _loop = loop; }
	bool IsLoop() { return _loop; }

	void SetTexture(shared_ptr <Texture> texture) { _texture = texture; }
	shared_ptr<Texture> GetTexure() { return _texture; } 
	Vec2 GetTextureSize();

	const Keyframe& GetKeyframe(int32 index); 
	int32 GetKeyframeCount(); 
	void AddKeyframe(const Keyframe& keyframe);

private:
	bool _loop = false; 
	shared_ptr<Texture> _texture; 
	vector<Keyframe> _keyframes; 

};

