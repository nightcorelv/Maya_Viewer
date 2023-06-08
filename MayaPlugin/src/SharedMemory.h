#pragma once
#include <string>
#include <Windows.h>

class SharedMemory final
{
	HANDLE handle;
public:

	size_t size;
	std::wstring name;
	char* message;

	void Create(std::wstring name, size_t size = 150)
	{
		if (size == 0)
			size = 150;
		this->name = name;
		size *= 1048576;// 1MB = 1048576 = (1 << 10)
		this->size = size;
		handle = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (unsigned long)size, name.c_str());
		message = (char*)MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, this->size);
	}
	void Open(std::wstring name)
	{
		this->name = name;
		handle = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, name.c_str());
		message = (char*)MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	}

	~SharedMemory()
	{
		UnmapViewOfFile(message);
		CloseHandle(handle);
	}
};

