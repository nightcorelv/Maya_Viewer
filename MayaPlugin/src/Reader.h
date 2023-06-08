#pragma once
#include <cstdint>
#include <string>
#include <Windows.h>

#include "SharedMemory.h"
#include "ControlMemory.h"
#include "Mutex.h"
#include "Structs.h"


class Reader
{
	inline static SharedMemory sharedMemory;
	inline static ControlMemory controlMemory;
	inline static Mutex mutex;

public:

	static void Init()
	{
		#define readIndex controlMemory.info->readIndex
		sharedMemory.Open(L"shared_memory_name");
		controlMemory.Open(L"control_memory_name");
		mutex.Create(L"mutex_name");

		// reset to default value
		readIndex = 0;
	}

	static void Read(StructHeader& header, void* data)
	{
		#define writeIndex controlMemory.info->writeIndex
		#define readIndex controlMemory.info->readIndex
		#define freeMemory controlMemory.info->freeMemory
		#define totalMemory controlMemory.info->totalMemory
		#define structSize header.dataSize + sizeof(header)

		mutex.Lock();

		// if any data didnt read
		if (readIndex != writeIndex)
		{
			// read header
			memcpy(&header, &sharedMemory.message[readIndex], sizeof(header));
			readIndex += sizeof(header);

			// read data
			memcpy(data, &sharedMemory.message[readIndex], header.dataSize);
			readIndex += header.dataSize;

			// if all data is read(up to date)
			if (readIndex == writeIndex)
			{
				// reset to default value
				writeIndex = 0;
				readIndex = 0;
				freeMemory = totalMemory;
			}

		}

		mutex.Unlock();
	}

	static void ReadHeader(StructHeader& header)
	{
		#define writeIndex controlMemory.info->writeIndex
		#define readIndex controlMemory.info->readIndex
		#define freeMemory controlMemory.info->freeMemory
		#define totalMemory controlMemory.info->totalMemory
		#define structSize header.dataSize + sizeof(header)

		mutex.Lock();

		// if any data didnt read
		if (readIndex != writeIndex)
		{
			// read header
			memcpy(&header, &sharedMemory.message[readIndex], sizeof(header));
			readIndex += sizeof(header);

			// if all data is read(up to date)
			if (readIndex == writeIndex)
			{
				// reset to default value
				writeIndex = 0;
				readIndex = 0;
				freeMemory = totalMemory;
			}

		}

		mutex.Unlock();

	}
	static void ReadData(StructHeader& header, void* data)
	{
		#define writeIndex controlMemory.info->writeIndex
		#define readIndex controlMemory.info->readIndex
		#define freeMemory controlMemory.info->freeMemory
		#define totalMemory controlMemory.info->totalMemory
		#define structSize header.dataSize + sizeof(header)

		mutex.Lock();

		// if any data didnt read
		if (readIndex != writeIndex)
		{

			// read data
			memcpy(data, &sharedMemory.message[readIndex], header.dataSize);
			readIndex += header.dataSize;

			// if all data is read(up to date)
			if (readIndex == writeIndex)
			{
				// reset to default value
				writeIndex = 0;
				readIndex = 0;
				freeMemory = totalMemory;
			}

		}

		mutex.Unlock();

	}

	static bool AllDataIsReaded()
	{
		#define writeIndex controlMemory.info->writeIndex
		#define readIndex controlMemory.info->readIndex

		return readIndex == writeIndex;
	}
};