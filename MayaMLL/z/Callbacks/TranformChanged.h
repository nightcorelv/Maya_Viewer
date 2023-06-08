#pragma once
#include "../SharedMemory/Writer.h"
#include <vector>
#include <string>
using namespace std;

void GetAllChildren(MObject& root, std::vector<MObject>& childList)
{
	MFnDagNode rootDag(root);

	//loop all children
	for (int i = 0; i < rootDag.childCount(); i++)
	{
		MObject* child = &childList.emplace_back(rootDag.child(i));
		if (MFnDagNode(*child).childCount() > 0)
		{
			GetAllChildren(*child, childList);
		}
	}

}

void TranformChanged(MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* x)
{

	if (msg & MNodeMessage::AttributeMessage::kAttributeSet)
	{
		MObject node(plug.node());

		StructHeader header{ .dataSize = sizeof(worldMatrix), .id = TransformChanged };

		MMatrix m = MDagPath::getAPathTo(node).inclusiveMatrix();

		worldMatrix wm;
		strcpy_s(wm.name, MFnDagNode(node).name().asChar());
		wm.m[0] = (float)m.matrix[0][0];
		wm.m[1] = (float)m.matrix[0][1];
		wm.m[2] = (float)m.matrix[0][2];
		wm.m[3] = (float)m.matrix[0][3];
		wm.m[4] = (float)m.matrix[1][0];
		wm.m[5] = (float)m.matrix[1][1];
		wm.m[6] = (float)m.matrix[1][2];
		wm.m[7] = (float)m.matrix[1][3];
		wm.m[8] = (float)m.matrix[2][0];
		wm.m[9] = (float)m.matrix[2][1];
		wm.m[10] = (float)m.matrix[2][2];
		wm.m[11] = (float)m.matrix[2][3];
		wm.m[12] = (float)m.matrix[3][0];
		wm.m[13] = (float)m.matrix[3][1];
		wm.m[14] = (float)m.matrix[3][2];
		wm.m[15] = (float)m.matrix[3][3];

		cout << "Parent name:" << wm.name << " , " << wm.m[0] << " , " << wm.m[1] << " , " << wm.m[2] << " , " << wm.m[3] << endl;
		cout << "Parent name:" << wm.name << " , " << wm.m[4] << " , " << wm.m[5] << " , " << wm.m[6] << " , " << wm.m[7] << endl;
		cout << "Parent name:" << wm.name << " , " << wm.m[8] << " , " << wm.m[9] << " , " << wm.m[10] << " , " << wm.m[11] << endl;
		cout << "Parent name:" << wm.name << " , " << wm.m[12] << " , " << wm.m[13] << " , " << wm.m[14] << " , " << wm.m[15] << endl;

		Writer::Send(header, &wm);


		// children arr
		std::vector<MObject> children;
		// get all chidren to arr
		GetAllChildren(node, children);

		std::vector<MMatrix> childWorldM;
		std::vector<std::string> childName;

		//put children name and worldM into array
		for (int i = 1; i < children.size(); i++)
		{
			// if child node is mesh
			if (children[i].hasFn(MFn::kMesh))
			{
				childName.emplace_back(MFnDagNode(children[i - 1]).name().asChar());
				childWorldM.emplace_back(MDagPath::getAPathTo(children[i]).inclusiveMatrix());
			}
		}

		//send children hieracy to shared memory
		for (int i = 0; i < childName.size(); i++)
		{
			m = childWorldM[i];

			strcpy_s(wm.name, childName[i].c_str());
			wm.m[0] = (float)m.matrix[0][0];
			wm.m[1] = (float)m.matrix[0][1];
			wm.m[2] = (float)m.matrix[0][2];
			wm.m[3] = (float)m.matrix[0][3];
			wm.m[4] = (float)m.matrix[1][0];
			wm.m[5] = (float)m.matrix[1][1];
			wm.m[6] = (float)m.matrix[1][2];
			wm.m[7] = (float)m.matrix[1][3];
			wm.m[8] = (float)m.matrix[2][0];
			wm.m[9] = (float)m.matrix[2][1];
			wm.m[10] = (float)m.matrix[2][2];
			wm.m[11] = (float)m.matrix[2][3];
			wm.m[12] = (float)m.matrix[3][0];
			wm.m[13] = (float)m.matrix[3][1];
			wm.m[14] = (float)m.matrix[3][2];
			wm.m[15] = (float)m.matrix[3][3];

			cout << "child name:" << wm.name << " , " << wm.m[0] << " , " << wm.m[1] << " , " << wm.m[2] << " , " << wm.m[3] << endl;
			cout << "child name:" << wm.name << " , " << wm.m[4] << " , " << wm.m[5] << " , " << wm.m[6] << " , " << wm.m[7] << endl;
			cout << "child name:" << wm.name << " , " << wm.m[8] << " , " << wm.m[9] << " , " << wm.m[10] << " , " << wm.m[11] << endl;
			cout << "child name:" << wm.name << " , " << wm.m[12] << " , " << wm.m[13] << " , " << wm.m[14] << " , " << wm.m[15] << endl;

			Writer::Send(header, &wm);
		}

	}

}