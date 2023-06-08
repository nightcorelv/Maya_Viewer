#pragma once
#include "../SharedMemory/Writer.h"

void NodeNameChanged(MObject& node, const MString& str, void* clientData)
{
	if ( node.hasFn(MFn::kLambert))
	{
		MFnDependencyNode dep(node);
		cout << "current dep name : " << dep.name().asChar() << endl;
		cout << "old name : " << str.asChar() << endl;

		namesChanged nameStruct;
		strcpy_s(nameStruct.name, dep.name().asChar());
		strcpy_s(nameStruct.previousName, str.asChar());

		//Send header
		StructHeader Sheader;
		Sheader.dataSize = sizeof(namesChanged);
		Sheader.id = DataType::NameChanged;
		Writer::Send(Sheader, &nameStruct);
	}
	else if(node.hasFn(MFn::kTransform))
	{
		MFnDagNode dag(node);
		cout << "current name : " << dag.name().asChar() << endl;
		cout << "old name : " << str.asChar() << endl;

		namesChanged nameStruct;
		strcpy_s(nameStruct.name, dag.name().asChar());
		strcpy_s(nameStruct.previousName, str.asChar());

		//Send header
		StructHeader Sheader;
		Sheader.dataSize = sizeof(namesChanged);
		Sheader.id = DataType::NameChanged;
		Writer::Send(Sheader, &nameStruct);
	}
}