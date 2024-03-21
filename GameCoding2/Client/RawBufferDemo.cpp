#include "pch.h"
#include "RawBufferDemo.h"
#include "RawBuffer.h"

void RawBufferDemo::Init()
{
	_shader = make_shared<Shader>(L"24. RawBufferDemo.fx");

	// 하나의 쓰레드 그룹 내에서 운형할 쓰레드 개수 
	uint32 count = 10 * 8 * 3; // MSN 공식 문서에 이렇게 되어 있어서 

	shared_ptr<RawBuffer> rawBuffer = make_shared<RawBuffer>(nullptr, 0, sizeof(Output) * count);

	_shader->GetUAV("Output")->SetUnorderedAccessView(rawBuffer->GetUAV().Get());

	// 원래는 draw 계열 함수를 썼는데 얘는 그냥 CompeuteShader를 실행하세요가 Dispatch로 실행된다
	// x, y, z 쓰레드 그룹 지정
	_shader->Dispatch(0, 0, 1, 1, 1); // 수치들이 ComputInput 여기의 값들과 연관이 있다. 

	// 각각 어떤 값들을 리턴 했는지 로그를 찍어 본다. 
	vector<Output> outputs(count);
	rawBuffer->CopyFromOutput(outputs.data());

	// 엑셀로 만들어서 살펴보는 코드
	FILE* file;
	::fopen_s(&file, "../RawBuffer.csv", "w");

	::fprintf
	(
		file,
		"GroupID(X),GroupID(Y),GroupID(Z),GroupThreadID(X),GroupThreadID(Y),GroupThreadID(Z),DispatchThreadID(X),DispatchThreadID(Y),DispatchThreadID(Z),GroupIndex\n"
	);

	for (uint32 i = 0; i < count; i++)
	{
		const Output& temp = outputs[i];

		::fprintf
		(
			file,
			"%d,%d,%d,	%d,%d,%d,	%d,%d,%d,	%d\n",
			temp.groupID[0], temp.groupID[1], temp.groupID[2],
			temp.groupThreadID[0], temp.groupThreadID[1], temp.groupThreadID[2],
			temp.dispatchThreadID[0], temp.dispatchThreadID[1], temp.dispatchThreadID[2],
			temp.groupIndex
		);
	}

	::fclose(file);
}

void RawBufferDemo::Update()
{

}

void RawBufferDemo::Render()
{
}
