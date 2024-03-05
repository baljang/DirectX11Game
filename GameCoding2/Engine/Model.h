#pragma once

struct ModelBone;
struct ModelMesh;
struct ModelAnimation; 

class Model : public enable_shared_from_this<Model> // 이걸 안붙이면 this 포인터로 사용할 수 없다. 
{
public:
	Model();
	~Model();

public:
	// 핵심적인 함수이다. 이제 거꾸로 하는 거 ExportModelData에서 우리만의 포멧으로 만들어진 메쉬랑 xml 포멧으로 들고 있는 
	// 그거를 다시 여기서 로드해서 메모리에 채워주는 작업을 해줘야 한다. fbx에 있던 거를 로드하고 정지해서 저장하고 다시 그걸 로드하는 단계
	// xml로 material은 되어 있으니까 그 xml의 그 형식을 그대로 맞춰가지고 다시 거꾸로 순서 맞춰가지고 하나씩 긁어서 읽어오는 작업을 해줘야 한다.
	void ReadMaterial(wstring filename);
	void ReadModel(wstring filename);
	void ReadAnimation(wstring filename); 

	uint32 GetMaterialCount() { return static_cast<uint32>(_materials.size()); }
	vector<shared_ptr<Material>>& GetMaterials() { return _materials; }
	shared_ptr<Material> GetMaterialByIndex(uint32 index) { return _materials[index]; }
	shared_ptr<Material> GetMaterialByName(const wstring& name);

	uint32 GetMeshCount() { return static_cast<uint32>(_meshes.size()); }
	vector<shared_ptr<ModelMesh>>& GetMeshes() { return _meshes; }
	shared_ptr<ModelMesh> GetMeshByIndex(uint32 index) { return _meshes[index]; }
	shared_ptr<ModelMesh> GetMeshByName(const wstring& name);

	uint32 GetBoneCount() { return static_cast<uint32>(_bones.size()); }
	vector<shared_ptr<ModelBone>>& GetBones() { return _bones; }
	shared_ptr<ModelBone> GetBoneByIndex(uint32 index) { return (index < 0 || index >= _bones.size() ? nullptr : _bones[index]); }
	shared_ptr<ModelBone> GetBoneByName(const wstring& name);

	uint32 GetAnimationCount() { return _animations.size(); }
	vector<shared_ptr<ModelAnimation>>& GetAnimations() { return _animations; }
	shared_ptr<ModelAnimation> GetAnimationByIndex(UINT index) { return (index < 0 || index >= _animations.size()) ? nullptr : _animations[index]; }
	shared_ptr<ModelAnimation> GetAnimationByName(wstring name);

private:
	void BindCacheInfo();

private:
	wstring _modelPath = L"../Resources/Models/";
	wstring _texturePath = L"../Resources/Textures/";
	// 애니메이션까지 하고 나중에 리소스매니져를 통해 하도록 만들어 줘야 한다.

private:
	shared_ptr<ModelBone> _root;
	vector<shared_ptr<Material>> _materials; // mesh를 어떻게 그려줘야 할지
	vector<shared_ptr<ModelBone>> _bones; // // mesh끼리 서로 상하 관계나 대칭구조
	vector<shared_ptr<ModelMesh>> _meshes; 
	vector<shared_ptr<ModelAnimation>> _animations; 
};

 