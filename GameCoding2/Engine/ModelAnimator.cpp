#include "pch.h"
#include "ModelAnimator.h"
#include "Material.h"
#include "ModelMesh.h"
#include "Model.h"
#include "ModelAnimation.h"

ModelAnimator::ModelAnimator(shared_ptr<Shader> shader)
	: Super(ComponentType::Animator), _shader(shader)
{
	// TEST
	_tweenDesc.next.animIndex = rand() % 3; // 다음 애니메이션을 무엇으로 틀지
	_tweenDesc.tweenSumTime += rand() % 100; // 현재 시간이 얼마나 경과 되었는지 
}

ModelAnimator::~ModelAnimator()
{
}

void ModelAnimator::SetModel(shared_ptr<Model> model)
{
	_model = model; 

	const auto& materials = _model->GetMaterials();
	for (auto& material : materials)
	{
		material->SetShader(_shader);
	}
}

void ModelAnimator::Update()
{


}

void ModelAnimator::UpdateTweenData()
{
	TweenDesc& desc = _tweenDesc;

	desc.curr.sumTime += DT;
	// 현재 애니메이션
	{
		shared_ptr<ModelAnimation> currentAnim = _model->GetAnimationByIndex(desc.curr.animIndex);
		if (currentAnim)
		{
			// 시간이 흐름에 따라서 프레임을 바꿔준다.
			float timePerFrame = 1 / (currentAnim->frameRate * desc.curr.speed);
			if (desc.curr.sumTime >= timePerFrame)
			{
				desc.curr.sumTime = 0;
				desc.curr.currFrame = (desc.curr.currFrame + 1) % currentAnim->frameCount;
				desc.curr.nextFrame = (desc.curr.currFrame + 1) % currentAnim->frameCount;
			}

			desc.curr.ratio = (desc.curr.sumTime / timePerFrame);
		}
	}

	// 다음 애니메이션이 예약 되어 있다면 
	if (desc.next.animIndex >= 0)
	{
		desc.tweenSumTime += DT;
		desc.tweenRatio = desc.tweenSumTime / desc.tweenDuration;

		if (desc.tweenRatio >= 1.f)
		{
			// 애니메이션 교체 성공
			desc.curr = desc.next;
			desc.ClearNextAnim();
		}
		else
		{
			// 프레임 교체
			shared_ptr<ModelAnimation> nextAnim = _model->GetAnimationByIndex(desc.next.animIndex);
			desc.next.sumTime += DT;

			float timePerFrame = 1.f / (nextAnim->frameRate * desc.next.speed);

			if (desc.next.ratio >= 1.f)
			{
				desc.next.sumTime = 0;

				desc.next.currFrame = (desc.next.currFrame + 1) % nextAnim->frameCount;
				desc.next.nextFrame = (desc.next.currFrame + 1) % nextAnim->frameCount;
			}

			desc.next.ratio = desc.next.sumTime / timePerFrame;
		}
	}
}

void ModelAnimator::RenderInstancing(shared_ptr<class InstancingBuffer>& buffer)
{
	if (_model == nullptr)
		return;

	// TODO
	if (_texture == nullptr)
		CreateTexture();	

	// SRV를 통해 정보 전달
	_shader->GetSRV("TransformMap")->SetResource(_srv.Get());

	// Bones
	BoneDesc boneDesc;

	const uint32 boneCount = _model->GetBoneCount();

	for (uint32 i = 0; i < boneCount; i++)
	{
		shared_ptr<ModelBone> bone = _model->GetBoneByIndex(i);
		boneDesc.transform[i] = bone->transform;
		// 안채워 준 값은 쓰레기값이 들어가겠지만 쉐이더에서 사용 안할 예정이라서 일단은 냅둔다.
		// 깔끔하게 하려면 identity 행렬을 밀어 준다거나 하는 식으로 작업을 하면 된다.
	}
	RENDER->PushBoneData(boneDesc);

	const auto& meshes = _model->GetMeshes();
	for (auto& mesh : meshes)
	{
		if (mesh->material)
			mesh->material->Update();

		// BoneIndex
		_shader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		mesh->vertexBuffer->PushData(); 
		mesh->indexBuffer->PushData(); 

		buffer->PushData();  // world와 관련된 정보

		_shader->DrawIndexedInstanced(0, _pass, mesh->indexBuffer->GetCount(), buffer->GetCount()); 
	}
}

InstanceID ModelAnimator::GetInstanceID()
{
	return make_pair((uint64)_model.get(), (uint64)_shader.get()); 
}

void ModelAnimator::CreateTexture()
{
	if (_model->GetAnimationCount() == 0)
		return;

	_animTransforms.resize(_model->GetAnimationCount());

	for (uint32 i = 0; i < _model->GetAnimationCount(); i++)
		CreateAnimationTransform(i);

	// CreateTexture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_KEYFRAMES;
		desc.ArraySize = _model->GetAnimationCount();
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 16바이트
		desc.Usage = D3D11_USAGE_IMMUTABLE;	// 한번 설정하면 안고치겠다
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		const uint32 dataSize = MAX_MODEL_TRANSFORMS * sizeof(Matrix);
		const uint32 pageSize = dataSize * MAX_MODEL_KEYFRAMES;
		void* mallocPtr = ::malloc(pageSize * _model->GetAnimationCount());

		// 파편화된 데이터를 조립한다.
		for (uint32 c = 0; c < _model->GetAnimationCount(); c++)
		{
			uint32 startOffset = c * pageSize;

			BYTE* pageStartPtr = reinterpret_cast<BYTE*>(mallocPtr) + startOffset;

			for (uint32 f = 0; f < MAX_MODEL_KEYFRAMES; f++)
			{
				void* ptr = pageStartPtr + dataSize * f;
				::memcpy(ptr, _animTransforms[c].transforms[f].data(), dataSize);
			}
		}

		// 리소스 만들기
		vector<D3D11_SUBRESOURCE_DATA> subResources(_model->GetAnimationCount());

		for (uint32 c = 0; c < _model->GetAnimationCount(); c++)
		{
			void* ptr = (BYTE*)mallocPtr + c * pageSize;
			subResources[c].pSysMem = ptr;
			subResources[c].SysMemPitch = dataSize;
			subResources[c].SysMemSlicePitch = pageSize;
		}

		HRESULT hr = DEVICE->CreateTexture2D(&desc, subResources.data(), _texture.GetAddressOf());
		CHECK(hr);

		::free(mallocPtr);
	}

	// Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = _model->GetAnimationCount();

		HRESULT hr = DEVICE->CreateShaderResourceView(_texture.Get(), &desc, _srv.GetAddressOf());
		CHECK(hr);
	}
}

void ModelAnimator::CreateAnimationTransform(uint32 index)
{
	vector<Matrix> tempAnimBoneTransforms(MAX_MODEL_TRANSFORMS, Matrix::Identity); // 캐싱 용도로 사용한다.

	shared_ptr<ModelAnimation> animation = _model->GetAnimationByIndex(index); //작업해야 할 애니메이션

	for (uint32 f = 0; f < animation->frameCount; f++) // 프레임 순회
	{
		for (uint32 b = 0; b < _model->GetBoneCount(); b++) // 본 순회
		{
			shared_ptr<ModelBone> bone = _model->GetBoneByIndex(b); 
		
			Matrix matAnimation; 

			shared_ptr<ModelKeyframe> frame = animation->GetKeyframe(bone->name); 
			if (frame != nullptr)
			{
				ModelKeyframeData& data = frame->transforms[f]; // bone이름으로 찾은 행렬중에서 해당 프레임 정보에 접근

				Matrix S, R, T; 
				// 수학라이브러리에서 제공하는 S,R,T 구하는 함수들
				S = Matrix::CreateScale(data.scale.x, data.scale.y, data.scale.z); 
				R = Matrix::CreateFromQuaternion(data.rotation); 
				T = Matrix::CreateTranslation(data.translation.x, data.translation.y, data.translation.z); 

				matAnimation = S * R * T; 
				// 특정 본의 특정 프레임에 관련된 SRT를 구해왔다.

				// SRT를 곱한게 어떤 의미를 들고 있는 걸까. global로 넘어간 상태가 아니라 animation의 relative한 상태의 변환행렬이라고 볼 수 있다. 
				// 상위 부모로 넘어가고 싶은 그 상태라고 볼 수 있다. 
			}
			else
			{
				matAnimation = Matrix::Identity; 
			}

			// ! 여기서 해주고 싶은 건 parent를 곱해주고 싶다
			// 지금까지 구해왔던 애들 중에서 부모를 찾아서 거기다 기입하는 작업을 해봤다.
			// Converter::ReadModelData에서 좌표를 구해줄 때 Relative로 구해줬지만, 
			// 그걸 다시 global로 넘겨주기 위해 transform*matParent를 해서 저장을 해준 부분이랑 코드가 유사하게 흐른다. 
			// 지금 까지 구한 애들을 	vector<Matrix> tempAnimBoneTransforms(MAX_MODEL_TRANSFORMS, Matrix::Identity);  여기다가 일단 저장해 줄 것이다.
			
			// !!!!! 오늘의 핵심
			Matrix toRootMatrix = bone->transform; // bone->transform이 무언지 생각하면 사실상 global 변환 행렬이다. 유사한 코드인 Converter::ReadModelData 흐름 참조. 
			Matrix invGlobal = toRootMatrix.Invert(); // 거꾸로 relative한 영역으로 데리고 오고 싶다면 

			int32 parentIndex = bone->parentIndex; 

			// 애니메이션과 관련된 부분 연산
			Matrix matParent = Matrix::Identity; 
			if (parentIndex >= 0) // 즉 부모가 있다고 한다면
				matParent = tempAnimBoneTransforms[parentIndex];  // 구하는 거 마다 업데이트를 해주는 아이

			tempAnimBoneTransforms[b] = matAnimation * matParent; // matAnimatino은 SRT라 상위 부모로 가는 행렬, matParent는 나의 부모님에서 global까지 쭉 뚫고 올라가는 그 부분을 저장한 것
			// b를 0 부터 시작하는 이유
			// tempAnimBoneTransforms에 들어가는 정보들이 어떠한 뼈대를 정한다 해도 그 뼈대에서 global로 가는 그 행렬을 여기서 구해주게 된다. 
			// 관절을 기준으로 한 거에서 애니메이션이 틀어진 상태의 글로벌로 넘어가는 그 상태가 되는 것이기 때문에 
			// 최종적으로 결론을 내리자면 
			_animTransforms[index].transforms[f][b] = invGlobal * tempAnimBoneTransforms[b]; 
			// 중요한 건 왜 이렇게 되는지를 이해하는 게 중요하다. 		
		}
	}
}
