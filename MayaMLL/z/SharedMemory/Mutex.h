#pragma once
#include <Windows.h>
#include <string>

class Mutex final
{
	HANDLE handle;
public:
	std::wstring name;

	void Lock()
	{
		WaitForSingleObject(handle, INFINITE);
	}
	void Unlock()
	{
		ReleaseMutex(handle);
	}

	void Create(std::wstring name)
	{
		this->name = name;
		handle = CreateMutexW(nullptr, false, name.c_str());
	}

	~Mutex()
	{
		CloseHandle(handle);
	}
};
