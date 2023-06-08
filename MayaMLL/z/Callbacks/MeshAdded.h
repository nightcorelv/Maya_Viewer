#pragma once
#include "../SharedMemory/Writer.h"

void MeshAdded(MObject& node)
{
	MStatus status = MS::kSuccess;
	MFnMesh mesh(node, &status);

	if (status == MS::kSuccess)
	{
		std::cout << "###Adde mesh###: " << mesh.name() << std::endl;

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
			tempVert.position[0] = (float)vertexArray[vertexIndex[triangleIndex[i]]].x;
			tempVert.position[1] = (float)vertexArray[vertexIndex[triangleIndex[i]]].y;
			tempVert.position[2] = (float)vertexArray[vertexIndex[triangleIndex[i]]].z;

			//Normal
			tempVert.normal[0] = (float)normalArray[normalIndex[triangleIndex[i]]].x;
			tempVert.normal[1] = (float)normalArray[normalIndex[triangleIndex[i]]].y;
			tempVert.normal[2] = (float)normalArray[normalIndex[triangleIndex[i]]].z;

			//UV
			tempVert.uv[0] = (float)uArray[uvIndex[triangleIndex[i]]];
			tempVert.uv[1] = (float)vArray[uvIndex[triangleIndex[i]]];

			verticies.push_back(tempVert);
		}


		// ---------- Send Triangle -----------

		//Header to send
		MeshAddedHeader addedHeader;

		//Get connected shader engines
		MObjectArray connectedShaders;
		MIntArray indi;
		std::string name = "";
		mesh.getConnectedShaders(0, connectedShaders, indi);
		

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
			name = lambertNode.name().asChar();
		}

		//Remove shape from mesh name
		std::string meshName = mesh.name().asChar();
		int sindex = -1;
		sindex = meshName.find("Shape");
		if (sindex != -1)
		{
			meshName.erase(sindex, 5);
		}

		//Assemble Header
		strcpy_s(addedHeader.materialName, name.c_str());
		strcpy_s(addedHeader.name, meshName.c_str());
		addedHeader.nrOfVert = verticies.size();
		
		MMatrix m = MDagPath::getAPathTo(node).inclusiveMatrix();

		addedHeader.m[0] = (float)m.matrix[0][0];
		addedHeader.m[1] = (float)m.matrix[0][1];
		addedHeader.m[2] = (float)m.matrix[0][2];
		addedHeader.m[3] = (float)m.matrix[0][3];
		addedHeader.m[4] = (float)m.matrix[1][0];
		addedHeader.m[5] = (float)m.matrix[1][1];
		addedHeader.m[6] = (float)m.matrix[1][2];
		addedHeader.m[7] = (float)m.matrix[1][3];
		addedHeader.m[8] = (float)m.matrix[2][0];
		addedHeader.m[9] = (float)m.matrix[2][1];
		addedHeader.m[10] = (float)m.matrix[2][2];
		addedHeader.m[11] = (float)m.matrix[2][3];
		addedHeader.m[12] = (float)m.matrix[3][0];
		addedHeader.m[13] = (float)m.matrix[3][1];
		addedHeader.m[14] = (float)m.matrix[3][2];
		addedHeader.m[15] = (float)m.matrix[3][3];

		//Send header
		StructHeader Sheader;
		Sheader.dataSize = sizeof(MeshAddedHeader);
		Sheader.id = DataType::MeshAdd;
		Writer::Send(Sheader, &addedHeader);

		//Send Vertex data
		StructHeader Vheader;
		Vheader.dataSize = sizeof(vertex) * verticies.size();
		Vheader.id = DataType::Vertex;
		Writer::Send(Vheader, verticies.data());

		//print
		cout << "MeshName:" << addedHeader.name << endl;
		cout << "MaterialName:" << addedHeader.materialName << endl;
		cout << "VertexNr:" << addedHeader.nrOfVert << endl;
	}
}