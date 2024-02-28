#include "pch.h"
#include "Converter.h"
#include <filesystem>
#include "Utils.h"
#include "tinyxml2.h"
#include "FileUtils.h"

Converter::Converter()
{
	_importer = make_shared<Assimp::Importer>(); 
}

Converter::~Converter()
{
}

void Converter::ReadAssetFile(wstring file)
{
	wstring fileStr = _assetPath + file;

	auto p = std::filesystem::path(fileStr);
	assert(std::filesystem::exists(p));

	_scene = _importer->ReadFile(
		Utils::ToString(fileStr),
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	); // 이걸 매번 하기엔 속도가 느리다. 그래서 파일로 갖고 있는 거다.
	
	assert(_scene != nullptr);
}

void Converter::ExportModelData(wstring savePath)
{
	wstring finalPath = _modelPath + savePath + L".mesh"; // .data, .model 로 해도 상관 없다. 하나 골라서 사용하면 된다. 
	ReadModelData(_scene->mRootNode, -1, -1); 
	WriteModelFile(finalPath); 
}

void Converter::ExportMaterialData(wstring savePath)
{
	wstring finalPath = _texturePath + savePath + L".xml"; 
	ReadMaterialData();
	WriteMaterialData(finalPath); 
}

void Converter::ReadModelData(aiNode* node, int32 index, int32 parent)
{
	shared_ptr<asBone> bone = make_shared<asBone>(); 
	bone->index = index; // index는 파싱한 순서라고 보면 된다. -1부터 child로 갈 때 마다 번호 증가
	bone->parent = parent; 
	bone->name = node->mName.C_Str(); 
	 
	// Relative Transform
	Matrix transform(node->mTransformation[0]); // Matrix 생성자 중에서 주소 하나를 받는 버전
	// 4x4 16개 숫자를 알아서 복사해준다. 
	bone->transform = transform.Transpose();  // fbx 포멧에서는 순서가 뒤바뀌어 있기 때문에 뒤집어 줘야 한다. 
	// transform이 무엇을 기준으로 되어야 하는 것이냐? 최종 루트가 아닌 직속상관(부모)를 기준으로 하는 좌표를 나타낸다. 
	// 이거를 변환해서 최종루트를 기준으로 하는 것으로 변신이 하고 싶다면 정제를 해줄 필요가 있다. 

	// 여기서 matParent는 무엇을 기준으로 하는 transform일까? 
	// Root(Local) 까지 가야 할 경로를 미리 계산한 것이다
	Matrix matParent = Matrix::Identity; 
	if (parent >= 0) // index의 부모가 있다면 
		matParent = _bones[parent]->transform; // 부모부터 root까지 한번에 계산한 거 

	// Local (Root) Transform
	// 부모를 하나씩 타고 가면서 다 곱해주면 된다. 
	bone->transform = bone->transform * matParent; 

	_bones.push_back(bone); 
	// 새로 구한 신참이 들고 있는 transform은 relative로 들고 있는게 아니라 
	// 최상위 루트를 기준으로 하는 최종 물체의 transform을 구해 준 다음에 넣어준 것이다.
	// 신참도 자식이 있었다고 하면 자식코드도 다시 실행이 되면서 자식 또한 Relative로 찾을 것인데 
	// 그걸 다시 이미 구해준 한번에 root로 가는 transform을 곱해줘서 걔도 한번에 빠져나갈 수 있게 한 것이다.
	
	// 여기까지 하나의 노드에 대해 처리 

	// Mesh
	ReadMeshData(node, index); 

	// 재귀적인 코드
	for (uint32 i = 0; i < node->mNumChildren; i++)
		ReadModelData(node->mChildren[i], _bones.size(), index); 
	// parent에 자신의 인덱스를 넣고, 
	// 새로운 아이의 번호는 _bones.size()로 말 그대로 bones가 늘어날 때 마다 맨 마지막에 들어갈 자리가 자신의 id라고 볼 수 있다.
}

void Converter::ReadMeshData(aiNode* node, int32 bone)
{
	if (node->mNumMeshes < 1)
		return;
// mesh 카운트가 0이 아니라 하나라도 있다면 마지막 노드는 메쉬 정보를 나타내기 위해서 사용하는 것이다라는 걸 알 수 있다. 
// 예를 들면 House를 추출하다 말고 계층구조가 있는데 갑자기 뜬금없이 한 노드에서는  Cube라는 메쉬를 들고 있는 용도로 활용되거나 작동을 하거다
// 그래서 파싱을 하다 보면 Mesh와 관련된 정보가 하나 뜰텐데 그거를 파싱하는 코드를 여기에 넣어주면 된다. 
	shared_ptr<asMesh> mesh = make_shared<asMesh>();
	mesh->name = node->mName.C_Str();
	mesh->boneIndex = bone;  // 나중에 따로 연결해주는 거 보다 여기서 흐름별로 가면서 여기서 끼워주는 것도 좋을 방법이다. 

	// mesh가 하나만 있는 경우도 있겠지만 경우에 따라서 여러개 있는 경우가 있다. 
	// dragon도 파싱할 때 보면 submeshes라고 메쉬가 3개짜리로 만들어져 있는 것을 볼 수 있다. 
	// 3개로 관리할지 묶어서 관리할지 선택을 해야 하는데 한번에 그려줘야 한다면 indexBuffer를 만들어서 해야 하기 때문에 
	// 따로 관리하기 보다는 이걸 합쳐서 하나로 만들어서 관리하는 것도 방법이다.  
	// fbx가 상상이상으로 복잡하게 되어 있기 때문에 오늘 작성한 코드가 모든 fbx에서 돌아갈지도 검증이 안된 것이다. 
	// submesh가 여러개 있을 수 있으니 하나씩 순회를 하는 코드를 넣어준다. 
	for (uint32 i = 0; i < node->mNumMeshes; i++)
	{
		uint32 index = node->mMeshes[i];  // 인덱스 번호가 실제로 scene의 mesh번호가 된다. 
		// 설계도를 보면 child node가 index를 갖고 있을건데 그 index가 Scene이 들고 있었던 mMeshes의 원본의 번호라 써있다. 
		// child node에서 mMesh가 0, 1, 3 이면 처음에 Scene에서 mMeshes가 들고 있었던 0, 1, 3번째에 있었던 그 메쉬 정보를 추출해서 갖고오면 된다. 
		// index번호를 이용해서 scene에서 들고 있던 mesh에 겁근 하는 것이라 결론은 내릴 수 있다. 
		const aiMesh* srcMesh = _scene->mMeshes[index]; 

		// 다음은 원래 하던 거처럼 이름을 꺼내주고 하는 식으로 하면 된다. 
		// aiMesh* srcMesh 가 mesh를 그릴 때 어떤 material로 그려줘야 되는지 정보도 들고 있다. 
		// 어딘가에서 수동으로 해줬던 정보를 여기서 들고 있다. fbx에 기능이 많다는 거. 

		// Material Name
		const aiMaterial* material = _scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->materialName = material->GetName().C_Str();
		// asMesh의 string materialName을 알아야 이 이름을 찾아서 struct asMaterial의 name과 매핑을 시켜줄 수 있을 것이다. 
		// 이걸 이용해야지 나중에 어떤 메쉬를 그릴 때는 어떤 마테리얼을 이용할지에 대한 관계도가 그려진다.

		// 내용이 복잡해지는게 지금까지는 Mesh하나에 Material 하나 짝을 이뤄서 거기다 MeshRenderer로 그려줬지만
		// 이제는 하나의 물체에 Mesh가 여러개 될 수 있다는 거다. 그 Mesh마다 동일한 Material을 사용한다는 보장도 없다. 
		// 용이 10개짜리 Mesh로 이루어져 있는데 10개짜리 mesh마다 material이 다 다르다고 하면 
		// material 10개, mesh가 10개가 될 수 있고, 그거를 관리할 수 있어야 한다. 
		// 그렇기 때문에 관계도가 있다고 보면 된다. 

		const uint32 startVertex = mesh->vertices.size();
		// submeshes를 몽쳐서 관리해줄 때 통합을 위해 따로 계산해준다.
		// 지금까지 정점의 번호가 몇 번 이었는지를 여기에 적어 놓고
		// 그 다음에 실시간으로 추가해주고,
		// 그 다음에 index 번호를 계산할 때도 + startVertex를 해서 오프셋 만큼을 뒤로 당겨서 관리하고 있다. 

		for (uint32 v = 0; v < srcMesh->mNumVertices; v++)
		{
			// Vertex
			VertexType vertex; // VertexTextureNormalTangentBlendData에 필요한 내용을 채워준다
			::memcpy(&vertex.position, &srcMesh->mVertices[v], sizeof(Vec3));

			// UV
			if (srcMesh->HasTextureCoords(0))
				::memcpy(&vertex.uv, &srcMesh->mTextureCoords[0][v], sizeof(Vec2));

			// Normal
			if (srcMesh->HasNormals())
				::memcpy(&vertex.normal, &srcMesh->mNormals[v], sizeof(Vec3));

			// 오늘은 애니메이션은 들어가지 않기 때문에 필요한 것만 넣어서 vertices에 넣어준다.
			mesh->vertices.push_back(vertex);
		}

		// Index
		// 주의할 점은 submeshes를 3개짜리를 뭉쳐서 관리하다 보면 인덱스 번호가 겹치면 안된다.
		// 인덱스 번호를 구분하면서 늘리면서 관리를 해줄 필요가 있다. 
		for (uint32 f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (uint32 k = 0; k < face.mNumIndices; k++)
				mesh->indices.push_back(face.mIndices[k] + startVertex);	
			// +startVertex해서 index번호를 관리하고 있다. submeshes를 통합 관리할 때 index번호가 겹치지 않게 관리해 주는 것이다. 
		}

	}
	_meshes.push_back(mesh);
}

void Converter::WriteModelFile(wstring finalPath)
{
	auto path = filesystem::path(finalPath);

	// 폴더가 없으면 만든다.
	filesystem::create_directory(path.parent_path());

	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(finalPath, FileMode::Write);

	// Bone Data
	file->Write<uint32>(_bones.size());
	for (shared_ptr<asBone>& bone : _bones)
	{
		file->Write<int32>(bone->index);
		file->Write<string>(bone->name);
		file->Write<int32>(bone->parent);
		file->Write<Matrix>(bone->transform);
	}

	// Mesh Data
	file->Write<uint32>(_meshes.size());
	for (shared_ptr<asMesh>& meshData : _meshes)
	{
		file->Write<string>(meshData->name);
		file->Write<int32>(meshData->boneIndex);
		file->Write<string>(meshData->materialName);

		// Vertex Data
		file->Write<uint32>(meshData->vertices.size());
		file->Write(&meshData->vertices[0], sizeof(VertexType) * meshData->vertices.size());

		// Index Data
		file->Write<uint32>(meshData->indices.size());
		file->Write(&meshData->indices[0], sizeof(uint32) * meshData->indices.size());
	}
}


void Converter::ReadMaterialData()
{
	for (uint32 i = 0; i < _scene->mNumMaterials; i++)
	{
		aiMaterial* srcMaterial = _scene->mMaterials[i]; 

		shared_ptr<asMaterial> material = make_shared<asMaterial>(); 
		material->name = srcMaterial->GetName().C_Str(); 
		
		aiColor3D color; 
		// Ambient
		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material->ambient = Color(color.r, color.g, color.b, 1.f); 

		// Diffuse
		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->diffuse = Color(color.r, color.g, color.b, 1.f);

		// Specular
		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material->specular = Color(color.r, color.g, color.b, 1.f);
		srcMaterial->Get(AI_MATKEY_SHININESS, material->specular.w); // 세기 설정

		// Emissive
		srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		material->emissive = Color(color.r, color.g, color.b, 1.0f);


		// diffuse, specular, normal과 관련된 텍스쳐 
		aiString file;

		// Diffuse Texture
		srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		material->diffuseFile = file.C_Str(); // 파일 이름 넣는 건 옵션이다. 

		// Specular Texture
		srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->specularFile = file.C_Str();

		// Normal Texture
		srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->normalFile = file.C_Str();

		_materials.push_back(material);

	}
}

void Converter::WriteMaterialData(wstring finalPath)
{
	// ~~/House/House.xml
	auto path = filesystem::path(finalPath); 

	// 폴더가 없으면 만든다.
	filesystem::create_directory(path.parent_path()); 

	string folder = path.parent_path().string(); 

	shared_ptr<tinyxml2::XMLDocument> document = make_shared<tinyxml2::XMLDocument>();

	// xml 포멧으로 읽을 거라는 거
	tinyxml2::XMLDeclaration* decl = document->NewDeclaration(); 
	document->LinkEndChild(decl);

	// 정하는 .xml의 구조대로 넣어주면 된다. 
	tinyxml2::XMLElement* root = document->NewElement("Materials");
	document->LinkEndChild(root);

	for (shared_ptr<asMaterial> material : _materials)
	{
		tinyxml2::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		tinyxml2::XMLElement* element = nullptr;

		element = document->NewElement("Name");
		element->SetText(material->name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(folder, material->diffuseFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(folder, material->specularFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(folder, material->normalFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->ambient.x);
		element->SetAttribute("G", material->ambient.y);
		element->SetAttribute("B", material->ambient.z);
		element->SetAttribute("A", material->ambient.w);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->diffuse.x);
		element->SetAttribute("G", material->diffuse.y);
		element->SetAttribute("B", material->diffuse.z);
		element->SetAttribute("A", material->diffuse.w);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->specular.x);
		element->SetAttribute("G", material->specular.y);
		element->SetAttribute("B", material->specular.z);
		element->SetAttribute("A", material->specular.w);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive");
		element->SetAttribute("R", material->emissive.x);
		element->SetAttribute("G", material->emissive.y);
		element->SetAttribute("B", material->emissive.z);
		element->SetAttribute("A", material->emissive.w);
		node->LinkEndChild(element);
	}

	document->SaveFile(Utils::ToString(finalPath).c_str());
}

// 경로에 모아 놓는 함수 
string Converter::WriteTexture(string saveFolder, string file)
{
	string fileName = filesystem::path(file).filename().string(); 
	string folderName = filesystem::path(saveFolder).filename().string(); 

	const aiTexture* srcTexture = _scene->GetEmbeddedTexture(file.c_str()); 
	// 에셋 가끔 살펴보면 실제로 fbx파일에 포함이 되어서 텍스쳐가 들어가 있는 경우가 간혹 있다. 
	// 어지간해서는 텍스쳐 따로 있는 버전을 사용하는 걸 조금 더 권장한다. 이 편이 관리가 편하기 때문이다. 
	// 어떤 건 안에 들어있고 어떤건 안들어가 있고 하면 귀찮다. 
	// 경우에 따라서는 그런 경우에는 메모리 안에 있던 걸 끄집어 내서 별도의 파일로 만드는 경우도 더러 있다. 
	// 코드를 사용할지는 모르겠지만 일단 그렇게 하는 방식으로 넣어 놓고 분석은 해본다. 

	if (srcTexture) // fbx에 텍스쳐가 들어가 있는 경우
	{
		string pathStr = saveFolder + fileName;

		if (srcTexture->mHeight == 0) // 데이터가 1차원 배열 형태로 저장되어 있다면 바이너리 모드로 만든다.
		{
			shared_ptr<FileUtils> file = make_shared<FileUtils>();
			file->Open(Utils::ToWString(pathStr), FileMode::Write);
			file->Write(srcTexture->pcData, srcTexture->mWidth);
		}
		else // fbx에 1차원 배열이 아닌 2차원 텍스쳐가 있다면 내부에 있는 걸 끄집어 내서 별도의 파일로 만드는 경우
		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
			desc.Width = srcTexture->mWidth;
			desc.Height = srcTexture->mHeight;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_IMMUTABLE;

			D3D11_SUBRESOURCE_DATA subResource = { 0 };
			subResource.pSysMem = srcTexture->pcData;

			ComPtr<ID3D11Texture2D> texture;
			HRESULT hr = DEVICE->CreateTexture2D(&desc, &subResource, texture.GetAddressOf());
			CHECK(hr);

			DirectX::ScratchImage img;
			::CaptureTexture(DEVICE.Get(), DC.Get(), texture.Get(), img);

			// Save To File
			hr = DirectX::SaveToDDSFile(*img.GetImages(), DirectX::DDS_FLAGS_NONE, Utils::ToWString(fileName).c_str());
			CHECK(hr);
		}
	}
	else // fbx에 텍스쳐가 안들어가 있는 경우
	{
		string originStr = (filesystem::path(_assetPath) / folderName / file).string();
		Utils::Replace(originStr, "\\", "/");

		string pathStr = (filesystem::path(saveFolder) / fileName).string();
		Utils::Replace(pathStr, "\\", "/");

		::CopyFileA(originStr.c_str(), pathStr.c_str(), false);
	}

	return fileName;
}

