#include "pch.h"
#include "GroupDemo.h"
#include "RawBuffer.h"

void GroupDemo::Init()
{
	_shader = make_shared<Shader>(L"25. GroupDemo.fx");

	uint32 threadCount = 10 * 8 * 3;
	uint32 groupCount = 2 * 1 * 1; 
	uint32 count = groupCount * threadCount; //최종적 스레드 개수

	vector<Input> inputs(count); 
	for (int i = 0; i < count; i++)
		inputs[i].value = rand() % 10000; 

	shared_ptr<RawBuffer> rawBuffer = make_shared<RawBuffer>(inputs.data(), sizeof(Input) * count, sizeof(Output) * count);

	_shader->GetSRV("Input")->SetResource(rawBuffer->GetSRV().Get()); 
	_shader->GetUAV("Output")->SetUnorderedAccessView(rawBuffer->GetUAV().Get());

	// 원래는 draw 계열 함수를 썼는데 얘는 그냥 CompeuteShader를 실행하세요가 Dispatch로 실행된다
	// x, y, z 쓰레드 그룹 지정
	_shader->Dispatch(0, 0, 2, 1, 1); // 수치들이 ComputInput 여기의 값들과 연관이 있다. 

	// 각각 어떤 값들을 리턴 했는지 로그를 찍어 본다. 
	vector<Output> outputs(count);
	rawBuffer->CopyFromOutput(outputs.data());

	// 엑셀로 만들어서 살펴보는 코드
	FILE* file;
	::fopen_s(&file, "../RawBuffer.csv", "w");

	::fprintf
	(
		file,
		"GroupID(X),GroupID(Y),GroupID(Z),GroupThreadID(X),GroupThreadID(Y),GroupThreadID(Z),DispatchThreadID(X),DispatchThreadID(Y),DispatchThreadID(Z),GroupIndex, Value\n"
	);

	for (uint32 i = 0; i < count; i++)
	{
		const Output& temp = outputs[i];

		::fprintf
		(
			file,
			"%d,%d,%d,	%d,%d,%d,	%d,%d,%d,	%d,%f\n",
			temp.groupID[0], temp.groupID[1], temp.groupID[2],
			temp.groupThreadID[0], temp.groupThreadID[1], temp.groupThreadID[2],
			temp.dispatchThreadID[0], temp.dispatchThreadID[1], temp.dispatchThreadID[2],
			temp.groupIndex, temp.value
		);
	}

	::fclose(file);
}

void GroupDemo::Update()
{

}

void GroupDemo::Render()
{
}
