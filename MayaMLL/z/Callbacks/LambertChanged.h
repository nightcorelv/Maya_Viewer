#pragma once
#include "../SharedMemory/Writer.h"

void LambertChanged(MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* x)
{
	MStatus status = MS::kSuccess;
	if (msg & MNodeMessage::AttributeMessage::kAttributeSet)
	{
		

		//MFnLambertShader lambert(plug.node());

		// color
		if (plug.partialName() == "c") 
		{
			//cout << "edited color" << lambert.color() << endl;
			if (plug.isConnected())
			{
				cout << "is connedted file" << endl;
			}
		}

		// transparency
		else if (plug.partialName() == "it") 
		{
			cout << "edited transparency" << endl;
		}

		// transparency
		else if (plug.partialName() == "dc")
		{
			cout << "edited diffuse" << endl;
		}

		// ambient
		else if (plug.partialName() == "ambc") 
		{
			cout << "edited ambient" << endl;
		}

		// incandescence
		else if (plug.partialName() == "ic") 
		{
			cout << "edited incandescence" << endl;
		}

	}

	if (msg & MNodeMessage::AttributeMessage::kConnectionMade)
	{
		// this lambert node has color texture
		if (plug.partialName() == "c" && otherPlug.partialName() == "oc")
		{
			cout << "lambert node Color plug made connection with file node Out Color plug " << endl;
		}

		// this lambert node has normal map
		if (plug.partialName() == "n" && otherPlug.partialName() == "o")
		{
			cout << "lambert node Normal plug made connedtion with Normal plug" << endl;
		}

	}
	if (msg & MNodeMessage::AttributeMessage::kConnectionBroken && plug.partialName() == "n")
	{
		cout << "###Plug###!!!: " << plug.name() << endl;
		cout << "###Plug###!!!: " << plug.partialName() << endl;
		cout << "OtherPlug: " << otherPlug.name() << endl;
	}


	if (msg & MNodeMessage::AttributeMessage::kConnectionBroken && plug.partialName() == "c" || msg & MNodeMessage::AttributeMessage::kConnectionBroken && plug.partialName() == "n" || msg & MNodeMessage::AttributeMessage::kConnectionMade && plug.partialName() == "n" || msg & MNodeMessage::AttributeMessage::kConnectionMade && plug.partialName() == "c")
	{


		//Variables 
		std::string filepath = "";
		std::string normalfilepath = "";
		MFnLambertShader lambert(plug.node());



		//Find Color plug
		MFnDependencyNode lambertNode(plug.node());
		MPlug colorPlug = lambertNode.findPlug("color", &status);
		if (status == MS::kSuccess)
		{
			//Get incomming connection
			MPlugArray fileArr;
			colorPlug.connectedTo(fileArr, true, false, &status);

			// if connected to file
			for (int j = 0; j < fileArr.length(); j++)
			{
				//Get dependency Node of File Node
				MFnDependencyNode fileNode(fileArr[j].node());

				//Get plug with the file path
				MPlug filepathPlug = fileNode.findPlug("fileTextureName", &status);
				if (status == MS::kSuccess)
				{
					MString tstring;
					filepathPlug.getValue(tstring);
					filepath = tstring.asChar();
				}
			}
		}

		//Find Normal plug
		MPlug normalPlug = lambertNode.findPlug("normalCamera", &status);
		if (status == MS::kSuccess)
		{
			//Get incomming connection
			MPlugArray bumpArr;
			normalPlug.connectedTo(bumpArr, true, false, &status);

			// if connected to bump2d object
			for (int i = 0; i < bumpArr.length(); i++)
			{
				MFnDependencyNode bumpNode(bumpArr[i].node());
				MPlug toFilePlug = bumpNode.findPlug("bumpValue", &status);
				if (status == MS::kSuccess)
				{
					//Get incomming connection
					MPlugArray fileArr;
					toFilePlug.connectedTo(fileArr, true, false, &status);

					// if connected to file
					for (int j = 0; j < fileArr.length(); j++)
					{
						//Get dependency Node of File Node
						MFnDependencyNode fileNode(fileArr[j].node());

						//Get plug with the file path
						MPlug filepathPlug = fileNode.findPlug("fileTextureName", &status);
						if (status == MS::kSuccess)
						{
							MString tstring;
							filepathPlug.getValue(tstring);
							normalfilepath = tstring.asChar();
						}
					}
				}
			}
		}


		material mat;
		mat.ambientColor[0] = lambert.ambientColor().r;
		mat.ambientColor[1] = lambert.ambientColor().g;
		mat.ambientColor[2] = lambert.ambientColor().b;
		mat.color[0] = lambert.color().r;
		mat.color[1] = lambert.color().g;
		mat.color[2] = lambert.color().b;
		strcpy(mat.filepath, filepath.c_str());
		strcpy(mat.normalmapfilepath, normalfilepath.c_str());
		strcpy(mat.name, lambert.name().asChar());

		//Send
		StructHeader test;
		test.dataSize = sizeof(material);
		test.id = DataType::MaterialChanged;
		Writer::Send(test, &mat);


		//Print
		cout << "name: " << mat.name << endl;
		cout << "AmbientColor: " << mat.ambientColor[0] << " " << mat.ambientColor[1] << " " << mat.ambientColor[2] << endl;
		cout << "Color: " << mat.color[0] << " " << mat.color[1] << " " << mat.color[2] << endl;
		cout << "Filepath: " << mat.filepath << endl;
		cout << "normalpath: " << mat.normalmapfilepath << endl;
		cout << endl;
	}

	//cout << "###EvalPlug###: " << plug.name() << endl;
	//cout << "###EvalPlug###: " << plug.partialName() << endl;

	if (msg & MNodeMessage::AttributeMessage::kAttributeSet || plug.partialName() == "oc")
	{
	// ------------------------ Send Material ---------------------------


	//Variables 
		std::string filepath = "";
		std::string normalfilepath = "";
		MFnLambertShader lambert(plug.node());

		cout << "###Plug###: " << plug.name() << endl;
		cout << "###Plug###: " << plug.partialName() << endl;
		cout << "OtherPlug: " << otherPlug.name() << endl;

		//Find Color plug
		MFnDependencyNode lambertNode(plug.node());
		MPlug colorPlug = lambertNode.findPlug("color", &status);
		if (status == MS::kSuccess)
		{
			//Get incomming connection
			MPlugArray fileArr;
			colorPlug.connectedTo(fileArr, true, false, &status);

			// if connected to file
			for (int j = 0; j < fileArr.length(); j++)
			{
				//Get dependency Node of File Node
				MFnDependencyNode fileNode(fileArr[j].node());

				//Get plug with the file path
				MPlug filepathPlug = fileNode.findPlug("fileTextureName", &status);
				if (status == MS::kSuccess)
				{
					MString tstring;
					filepathPlug.getValue(tstring);
					filepath = tstring.asChar();
				}
			}
		}

		//Find Normal plug
		MPlug normalPlug = lambertNode.findPlug("normalCamera", &status);
		if (status == MS::kSuccess)
		{
			//Get incomming connection
			MPlugArray bumpArr;
			normalPlug.connectedTo(bumpArr, true, false, &status);

			// if connected to bump2d object
			for (int i = 0; i < bumpArr.length(); i++)
			{
				MFnDependencyNode bumpNode(bumpArr[i].node());
				MPlug toFilePlug = bumpNode.findPlug("bumpValue", &status);
				if (status == MS::kSuccess)
				{
					//Get incomming connection
					MPlugArray fileArr;
					toFilePlug.connectedTo(fileArr, true, false, &status);

					// if connected to file
					for (int j = 0; j < fileArr.length(); j++)
					{
						//Get dependency Node of File Node
						MFnDependencyNode fileNode(fileArr[j].node());

						//Get plug with the file path
						MPlug filepathPlug = fileNode.findPlug("fileTextureName", &status);
						if (status == MS::kSuccess)
						{
							MString tstring;
							filepathPlug.getValue(tstring);
							normalfilepath = tstring.asChar();
						}
					}
				}
			}
		}


		material mat;
		mat.ambientColor[0] = lambert.ambientColor().r;
		mat.ambientColor[1] = lambert.ambientColor().g;
		mat.ambientColor[2] = lambert.ambientColor().b;
		mat.color[0] = lambert.color().r;
		mat.color[1] = lambert.color().g;
		mat.color[2] = lambert.color().b;
		strcpy(mat.filepath, filepath.c_str());
		strcpy(mat.normalmapfilepath, normalfilepath.c_str());
		strcpy(mat.name, lambert.name().asChar());

		//Send
		StructHeader test;
		test.dataSize = sizeof(material);
		test.id = DataType::MaterialChanged;
		Writer::Send(test, &mat);


		//Print
		cout << "name: " << mat.name << endl;
		cout << "AmbientColor: " << mat.ambientColor[0] << " " << mat.ambientColor[1] << " " << mat.ambientColor[2] << endl;
		cout << "Color: " << mat.color[0] << " " << mat.color[1] << " " << mat.color[2] << endl;
		cout << "Filepath: " << mat.filepath << endl;
		cout << "normalpath: " << mat.normalmapfilepath << endl;
		cout << endl;
	}
}