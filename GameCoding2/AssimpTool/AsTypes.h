#pragma once

using VertexType = VertexTextureNormalTangentBlendData;
// 만약 생각이 바뀌면 얘만 교체해주면 된다. 

struct asBone // as가 붙은 건 우리만의 버전이다. 
{
	string name; 
	int32 index = -1;	// 몇 번째 뼈대인지 
	int32 parent = -1;  // 부모님은 몇번인지 파싱을 하는 도중에 번호를 지어서 관리 
	Matrix transform; // 계층구조 안에서 SRT 정보가 만들어진다.
};

struct asMesh
{
	string name; 
	aiMesh* mesh; 
	vector<VertexType> vertices; 
	vector<uint32> indices; 
	 
	int32 boneIndex; // 매핑할 때 계층구조에서 누구랑 연관 있는지, 어떤 뼈대에 붙어 있는지 추적
	string materialName; 
};

struct asMaterial
{
	string name; 
	Color ambient; 
	Color diffuse; 
	Color specular; 
	Color emissive; 
	string diffuseFile; // 이미지 파일 경로
	string specularFile; 
	string normalFile; 
};

// Animation

// 정점마다 -> (관절번호, 가중치)
struct asBlendWeight
{
	void Set(uint32 index, uint32 boneIndex, float weight)
	{
		float i = (float)boneIndex;
		float w = weight;

		switch (index)
		{
		case 0: indices.x = i; weights.x = w; break;
		case 1: indices.y = i; weights.y = w; break;
		case 2: indices.z = i; weights.z = w; break;
		case 3: indices.w = i; weights.w = w; break;
		}
	}

	Vec4 indices = Vec4(0, 0, 0, 0); // 쉐이더에 넘겨줄 때도 Vec4로 받아줄 것이기 때문
	Vec4 weights = Vec4(0, 0, 0, 0);	
};

struct asBoneWeights
{
	void AddWeights(uint32 boneIndex, float weight)
	{
		if (weight <= 0.0f)
			return;

		auto findIt = std::find_if(boneWeights.begin(), boneWeights.end(),
			[weight](const Pair& p) { return weight > p.second; }); // 가장 처음으로 weight이 다른 애 보단 큰 경우 return
	
		// (1, 0.4) (2, 0, 0.2) 이렇게 있을 때 (5, 0.5)를 넣는다 하면
		// (5, 0.5) (1, 0.4) (2, 0, 0.2) 이렇게 앞에 들어가게 된다. 가중치가 높은 애들을 앞에 배치하기 위해서 이렇게 해줬다.
		boneWeights.insert(findIt, Pair(boneIndex, weight)); 

	}

	// Vec4로 들고 있는게 아니라 나중에 asBlendWight에 넘겨줄 떄는 float4 끼워줘서 넘겨줘야 하기 때문에 asBlendWeight로 변환하는 함수를 하나 파줄 것이다.
	asBlendWeight GetBlendWeights()
	{
		asBlendWeight blendWeights;

		for (uint32 i = 0; i < boneWeights.size(); i++)
		{
			if (i >= 4)
				break;

			blendWeights.Set(i, boneWeights[i].first, boneWeights[i].second);
		}

		return blendWeights;
	}
	
	// 만약에 모든 정점들이 뼈대 4개의 영향을 받으면 좋겠지만, 어떤 애는 하나에만 영향을 받고, 어떤 애는 2개에만 영향을 받게 될 것이다.
	// 결국 최종적인 모든 애들의 합을 1로 맞춰주기 위해 연산을 해준다. 
	// (1, 0.3) (2, 0.2) 이 경우 
	// (1, 0.6) (2, 0.4) 로 해서 최종적인 합을 1로 구해주겠다가 핵심이다. 퍼센티지라고 보면 된다.
	void Normalize()
	{
		if (boneWeights.size() >= 4)
			boneWeights.resize(4); 

		float totalWeight = 0.f; 
		for (const auto& item : boneWeights)
			totalWeight += item.second; 

		float scale = 1.f / totalWeight; 
		for (auto& item : boneWeights)
			item.second *= scale; 
	}

	using Pair = pair<int32, float>; // boneIndex, weight
	vector<Pair> boneWeights; // 나중엔 4개 이상이면 정렬해서 가중치 높은 순서로 커트할 것이다. 
};