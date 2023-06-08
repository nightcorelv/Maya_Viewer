#pragma once
#include "../SharedMemory/Writer.h"

void MeshChanged(MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* x)
{
	MStatus status = MS::kSuccess;
	MFnMesh mesh(plug.node(), &status);

	if (msg & MNodeMessage::AttributeMessage::kConnectionMade && plug.partialName() == "iog" && otherPlug.partialName() == "dsm")
	{
		//Variables 
		std::string name = "";
	
		//Get connected shader engines
		MObjectArray connectedShaders;
		MIntArray indi;
		mesh.getConnectedShaders(0, connectedShaders, indi);
		if (connectedShaders.length() > 0)
		{
			//Get dependency node of first shader engine
			MObject shaderObj = connectedShaders[0];
			MPlug shadingEnginePlug = MFnDependencyNode(shaderObj).findPlug("surfaceShader", &status);

			//Get incomming connections
			MPlugArray SShaderConnections;
			shadingEnginePlug.connectedTo(SShaderConnections, true, false, &status);
			if (status == MS::kSuccess)
			{
				// dependency Node of material
				name = MFnDependencyNode(SShaderConnections[0].node()).name().asChar();

				std::string meshName = mesh.name().asChar();
				//Remove sg
				int sindex = -1;
				sindex = meshName.find("Shape");
				if (sindex != -1)
				{
					meshName.erase(sindex, 5);
				}

				swappedMaterial mat;
				strcpy_s(mat.meshName, meshName.c_str());
				strcpy_s(mat.newName, name.c_str());

				//Send
				StructHeader test;
				test.dataSize = sizeof(swappedMaterial);
				test.id = DataType::MaterialSwapped;
				Writer::Send(test, &mat);

				//Print
				cout << "Mesh name: " << mat.meshName << endl;
				cout << "New Material name: " << mat.newName << endl;
				cout << endl;
			}
		}

	
	}

	if (msg & MNodeMessage::AttributeMessage::kAttributeEval && plug.partialName() != "wm")
	{

		//Get connected shader engines
		MObjectArray connectedShaders;
		MIntArray indi;
		mesh.getConnectedShaders(0, connectedShaders, indi);
		if (connectedShaders.length() > 0)
		{
			std::string materialName;
			//Get dependency node of first shader engine
			MObject shaderObj = connectedShaders[0];
			MFnDependencyNode shaderNode(shaderObj);
			MPlug shadingEnginePlug = shaderNode.findPlug("surfaceShader", &status);
			if (status == MS::kSuccess)
			{
				//Get incomming connections
				MPlugArray SShaderConnections;
				shadingEnginePlug.connectedTo(SShaderConnections, true, false, &status);

				//Get dependency Node of material
				MFnDependencyNode lambertNode(SShaderConnections[0].node());
				materialName = lambertNode.name().asChar();
			}


			//Remove shape from mesh name
			std::string meshName = mesh.name().asChar();
			int sindex = -1;
			sindex = meshName.find("Shape");
			if (sindex != -1)
			{
				meshName.erase(sindex, 5);
			}

			// ---------- Triangle info ----------

			std::vector<vertex> verticies;

			//Get vertex information
			MPointArray vertexArray;
			MFloatVectorArray normalArray;
			MFloatArray uArray, vArray;
			mesh.getPoints(vertexArray, MSpace::kObject);
			mesh.getNormals(normalArray, MSpace::kObject);
			mesh.getUVs(uArray, vArray);

			//for indexing
			MIntArray triangelCount, triangleIndex;
			MIntArray vertexCount, vertexIndex;
			MIntArray normalCount, normalIndex;
			MIntArray uvCount, uvIndex;

			//get all indicies for a triangle and the verticies
			mesh.getTriangleOffsets(triangelCount, triangleIndex);
			mesh.getVertices(vertexCount, vertexIndex);
			mesh.getNormalIds(normalCount, normalIndex);
			mesh.getAssignedUVs(uvCount, uvIndex);

			for (int i = 0; i < triangleIndex.length(); i++)
			{
				vertex tempVert;

				//Pos
				tempVert.position[0] = vertexArray[vertexIndex[triangleIndex[i]]].x;
				tempVert.position[1] = vertexArray[vertexIndex[triangleIndex[i]]].y;
				tempVert.position[2] = vertexArray[vertexIndex[triangleIndex[i]]].z;

				//Normal
				tempVert.normal[0] = normalArray[normalIndex[triangleIndex[i]]].x;
				tempVert.normal[1] = normalArray[normalIndex[triangleIndex[i]]].y;
				tempVert.normal[2] = normalArray[normalIndex[triangleIndex[i]]].z;

				//UV
				tempVert.uv[0] = uArray[uvIndex[triangleIndex[i]]];
				tempVert.uv[1] = vArray[uvIndex[triangleIndex[i]]];

				verticies.push_back(tempVert);
			}

			//Header to send
			meshHeader tempHeader;

			//Assemble Header
			strcpy_s(tempHeader.materialName, materialName.c_str());
			strcpy_s(tempHeader.name, meshName.c_str());
			tempHeader.nrOfVert = verticies.size();

			//Send header
			StructHeader Sheader;
			Sheader.dataSize = sizeof(meshHeader);
			Sheader.id = DataType::MeshHeader;
			Writer::Send(Sheader, &tempHeader);

			//Send Vertex data
			StructHeader Vheader;
			Vheader.dataSize = sizeof(vertex) * verticies.size();
			Vheader.id = DataType::Vertex;
			Writer::Send(Vheader, verticies.data());

			//print
			cout << "MeshName:" << tempHeader.name << endl;
			cout << "MaterialName:" << tempHeader.materialName << endl;
			cout << "VertexNr:" << tempHeader.nrOfVert << endl;
			cout << endl;
		}
	}

}