#include "pch.h"
#include "FileUtils.h"

FileUtils::FileUtils()
{

}

FileUtils::~FileUtils()
{
	if (_handle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(_handle);
		_handle = INVALID_HANDLE_VALUE;
	}
}

void FileUtils::Open(wstring filePath, FileMode mode)
{
	if (mode == FileMode::Write)
	{
		// 이 handle에서 데이터를 꺼내 쓰면 된다. 
		// 새로 만든다.
		_handle = ::CreateFile
		( // ::의 의미는 만든게 아니라 API상에서 제공하는 것이란 의미
			filePath.c_str(),
			GENERIC_WRITE,
			0,
			nullptr,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
	}
	else
	{// 있는 것을 열어 주겠다. 없으면 따로 만들진 않겠다.
		_handle = ::CreateFile
		(
			filePath.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
	}

	assert(_handle != INVALID_HANDLE_VALUE); // 핸들에서 데이터를 꺼내 쓰면 된다.
}

void FileUtils::Write(void* data, uint32 dataSize)
{
	uint32 numOfBytes = 0;
	assert(::WriteFile(_handle, data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr));
}

void FileUtils::Write(const string& data)
{
	// 가변 데이터니 사이즈를 먼저 넣어준다.(문자열의 길이)
	uint32 size = (uint32)data.size();
	Write(size);

	if (data.size() == 0)
		return;

	// 사이즈에 해당하는 데이터를 밀어 넣는다.(문자열)
	Write((void*)data.data(), size);
}

void FileUtils::Read(void** data, uint32 dataSize)
{
	uint32 numOfBytes = 0;
	assert(::ReadFile(_handle, *data, dataSize, reinterpret_cast<LPDWORD>(&numOfBytes), nullptr));
}

void FileUtils::Read(OUT string& data)
{
	uint32 size = Read<uint32>();// 데이터 스트림의 저장 포맷이 첫 4바이트에 길이를 저장하기 때문

	if (size == 0)
		return;

	char* temp = new char[size + 1];
	temp[size] = 0;
	Read((void**)&temp, size);
	data = temp;
	delete[] temp;
}