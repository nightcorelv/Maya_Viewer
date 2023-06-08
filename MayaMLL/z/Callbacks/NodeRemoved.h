#pragma once
#include "../SharedMemory/Writer.h"

void NodeRemoved(MObject& node, void* clientData)
{
	if (node.hasFn(MFn::kDagNode))
	{
		if (node.hasFn(MFn::kMesh))
		{
			CallbackHandler::RemoveNameChangeCallback(node);
			CallbackHandler::RemoveChangedCallback(node);

			MFnDagNode removedNode(node);
			std::cout << "Removed mesh: " << removedNode.name() << std::endl;

			//Send
			StructHeader test;
			test.dataSize = sizeof(removedStruct);
			test.id = DataType::NodeRemove;

			removedStruct rem;
			string name = removedNode.name().asChar();
			int sindex = -1;
			sindex = name.find("Shape");
			if (sindex != -1)
			{
				name.erase(sindex, 5);
			}

			strcpy_s(rem.name, name.c_str());
			rem.id = MeshEnum;
			Writer::Send(test, &rem);
		}
		else if (node.hasFn(MFn::kTransform))
		{
			CallbackHandler::RemoveNameChangeCallback(node);
			CallbackHandler::RemoveChangedCallback(node);

			MFnDagNode removedNode(node);
			std::cout << "Removed transform: " << removedNode.name() << endl;
		}
		else if (node.hasFn(MFn::kCamera))
		{
			CallbackHandler::RemoveNameChangeCallback(node);
			CallbackHandler::RemoveChangedCallback(node);

			MFnDagNode removedNode(node);
			std::cout << "Removed camera: " << removedNode.name() << endl;
		}
	}
	else
	{

		if (node.hasFn(MFn::kLambert))
		{
			CallbackHandler::RemoveNameChangeCallback(node);
			CallbackHandler::RemoveChangedCallback(node);

			MFnDependencyNode removedNode(node);
			cout << "Removed Lambert: " << removedNode.name() << endl;

			//Send
			StructHeader test;
			test.dataSize = sizeof(removedStruct);
			test.id = DataType::NodeRemove;

			removedStruct rem;
			strcpy_s(rem.name, removedNode.name().asChar());
			rem.id = MaterialEnum;
			Writer::Send(test, &rem);
		}
		else if (node.hasFn(MFn::kFileTexture))
		{
			CallbackHandler::RemoveNameChangeCallback(node);
			CallbackHandler::RemoveChangedCallback(node);

			MFnDependencyNode removedNode(node);
			std::cout << "Removed FileTexture: " << removedNode.name() << endl;

		}
	}

}