#pragma once

// Write를 할건지 Read를 할건지
enum FileMode : uint8 
{
	Write, 
	Read,
}; 

class FileUtils
{
public: 
	FileUtils(); 
	~FileUtils(); 

	void Open(wstring filePath, FileMode mode); // mode에 읽는 용도인지 쓰는 용도인지
	
	// 일반적인 데이터는 이렇게 사이즈 별로 넣어준다.
	// 예외적인 경우는 string 같은 클래스의 경우는 동적 배열 느낌이라 여기서 막바로 사용하면 안된다.
	template<typename T>
	void Write(const T& data) // 복사 않고 참조
	{
		DWORD numOfBytes = 0;
		assert(::WriteFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr));
	}

	// 템플릿 특수화 문법: 템플릿을 사용하지만 string을 사용하는 경우에만 예외적으로 얘를 사용하겠다.
	template<>
	void Write<string>(const string& data)
	{
		return Write(data);
	}

	// vector나 string 같이 사이즈를 알 수 없는 경우
	void Write(void* data, uint32 dataSize);
	void Write(const string& data);

	// 전달 받은 data에 넣어주는 버전
	template<typename T>
	void Read(OUT T& data)
	{
		DWORD numOfBytes = 0;
		assert(::ReadFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr));
	}

	// 함수 안에서 선언한 data에 채워주고 리턴해 복사하는 버전
	template<typename T>
	T Read()
	{
		T data;
		Read(data);
		return data;
	}

	void Read(void** data, uint32 dataSize);
	void Read(OUT string& data);

private: 
	HANDLE _handle = INVALID_HANDLE_VALUE; // 파일 핸들 

};