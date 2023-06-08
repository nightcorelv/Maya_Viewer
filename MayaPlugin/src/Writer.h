#pragma once

#include "SharedMemory.h"
#include "ControlMemory.h"
#include "Mutex.h"
#include "../../Structs.h"


class Writer
{
	inline static SharedMemory sharedMemory;
	inline static ControlMemory controlMemory;
	inline static Mutex mutex;

public:

	static void Init()
	{
		sharedMemory.Create(L"shared_memory_name", 10);
		controlMemory.Create(L"control_memory_name", sharedMemory);
		mutex.Create(L"mutex_name");
	}

	static void Send(StructHeader header, void* data)
	{
		#define writeIndex controlMemory.info->writeIndex
		#define readIndex controlMemory.info->readIndex
		#define freeMemory controlMemory.info->freeMemory
		#define totalMemory controlMemory.info->totalMemory
		#define structSize header.dataSize + sizeof(header)

		mutex.Lock();

		// if enough if memory
		if (structSize < freeMemory - 1)
		{
			memcpy(sharedMemory.message + writeIndex, &header, sizeof(header));
			writeIndex += sizeof(header);

			memcpy(sharedMemory.message + writeIndex, data, header.dataSize);
			writeIndex += header.dataSize;

			freeMemory -= structSize;
		}

		//cout << writeIndex << " , " << readIndex << " , " << freeMemory << endl;

		mutex.Unlock();
	}
};