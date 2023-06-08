#pragma once
#include <cstdint>
#include <Windows.h>

#include "SharedMemory.h"

class ControlMemory final
{
	HANDLE handle;
public:

	struct Info
	{
		size_t readIndex = 0;
		size_t writeIndex = 0;
		size_t freeMemory = 0;
		size_t totalMemory = 0;
	};

	std::wstring name;
	char* data;

	Info* info;

	void Create(std::wstring name, SharedMemory& sharedMemory)
	{
		this->name = name;
		handle = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(Info), name.c_str());
		data = (char*)MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Info));

		info = (Info*)data;

		info->readIndex = 0;
		info->writeIndex = 0;
		info->freeMemory = sharedMemory.size;
		info->totalMemory = sharedMemory.size;

	}

	void Open(std::wstring name)
	{
		this->name = name;
		handle = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, name.c_str());
		data = (char*)MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Info));

		info = (Info*)data;
	}

	~ControlMemory()
	{
		UnmapViewOfFile(data);
		CloseHandle(handle);
	}

};