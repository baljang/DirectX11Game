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