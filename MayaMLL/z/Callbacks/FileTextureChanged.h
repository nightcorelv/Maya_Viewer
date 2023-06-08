#pragma once
#include "../SharedMemory/Writer.h"

void FileTextureChanged(MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* x)
{

	if (msg & MNodeMessage::AttributeMessage::kAttributeSet)
	{
		// fileTextureName, path
		if (plug.partialName() == "ftn")
		{
			cout << "Edited File Path" << endl;
		}
	}

	if (msg & MNodeMessage::AttributeMessage::kConnectionMade)
	{

		// this file node used as color texture
		if (plug.partialName() == "oc" && otherPlug.partialName() == "c")
		{
			cout << "file node Out Color plug made connection with Lambert Color " << endl;
		}

		// this file node used as normal map
		if (plug.partialName() == "oa" && otherPlug.partialName() == "bv")
		{
			cout << "file node Out Alpha plug med connedtion with Bumpvalue" << endl;
		}

	}

	if (msg & MNodeMessage::AttributeMessage::kConnectionMade && plug.partialName() == "oc")
	{
		cout << "////EvalPlug###: " << plug.name() << endl;
		cout << "////EvalPlug###: " << plug.partialName() << endl;
	}



	if (msg & MNodeMessage::AttributeMessage::kAttributeEval)
	{

		cout << "###EvalPlug###: " << plug.name() << endl;
		cout << "###EvalPlug###: " << plug.partialName() << endl;
		if (plug.partialName() == "ot" || plug.partialName() == "oa" || plug.partialName() == "oc")
		{

			// ------------------------ Send Material ---------------------------

			//Variables 
			MStatus status;
			MFnDependencyNode fileNode(plug.node());
			std::string filepath = "";
			std::string normalfilepath = "";
			std::string name = "";
			MColor savedAmbient;
			MColor savedColor;

			//Find out plug
			MPlug outColorPlug = fileNode.findPlug("outColor", &status);
			MPlug alphaPlug = fileNode.findPlug("outAlpha", &status);
			if (outColorPlug.isConnected())			//lambertNode
			{
				//Get plug with the file path
				MPlug filepathPlug = fileNode.findPlug("fileTextureName", &status);
				if (status == MS::kSuccess)
				{
					MString tstring;
					filepathPlug.getValue(tstring);
					filepath = tstring.asChar();
					cout << "File path Plug: " << tstring.asChar() << endl;
				}

				//Get incomming connection
				MPlugArray fileArr;
				outColorPlug.connectedTo(fileArr, false, true, &status);

				//if connected to file
				for (int i = 0; i < fileArr.length(); i++)
				{
					//Get dependency Node of incomming connection aka lambertNode
					MFnDependencyNode lambertNode(fileArr[i].node());
					MFnLambertShader lambert(fileArr[i].node());
					name = lambert.name().asChar();
					savedColor = lambert.color();
					savedAmbient = lambert.ambientColor();


					// -------- Get normal texture filename -------- //


					//Find Normal plug
					MPlug normalPlug = lambertNode.findPlug("normalCamera", &status);

					//Get incomming connection
					MPlugArray bumpArr;
					normalPlug.connectedTo(bumpArr, true, false, &status);

					// if connected to bump2d object
					for (int j = 0; j < bumpArr.length(); j++)
					{
						MFnDependencyNode bumpNode(bumpArr[j].node());
						MPlug toFilePlug = bumpNode.findPlug("bumpValue", &status);

						//Get incomming connection
						MPlugArray file2Arr;
						toFilePlug.connectedTo(file2Arr, true, false, &status);

						// if connected to file
						for (int k = 0; k < file2Arr.length(); k++)
						{
							MFnDependencyNode file2Node(file2Arr[k].node());

							//Get plug with the file path
							MPlug pathPlug = file2Node.findPlug("fileTextureName", &status);
							if (status == MS::kSuccess)
							{
								MString tstring;
								pathPlug.getValue(tstring);
								normalfilepath = tstring.asChar();
							}
						}
					}
				}
			}
			else if (alphaPlug.isConnected())		//bump2dNode
			{
				//Get plug with the file path
				MPlug filepathPlug = fileNode.findPlug("fileTextureName", &status);
				if (status == MS::kSuccess)
				{
					MString tstring;
					filepathPlug.getValue(tstring);
					normalfilepath = tstring.asChar();
					cout << "File path Plug: " << tstring.asChar() << endl;
				}

				//Get incomming connection
				MPlugArray fileArr;
				alphaPlug.connectedTo(fileArr, false, true, &status);

				//if connected to file
				for (int i = 0; i < fileArr.length(); i++)
				{
					//Get dependency Node of Bump Node
					MFnDependencyNode fileNode(fileArr[i].node());

					//Get out plug
					MPlug normalPlug = fileNode.findPlug("outNormal", &status);

					//Get incomming connection
					MPlugArray lambertArr;
					normalPlug.connectedTo(lambertArr, false, true, &status);
					for (int j = 0; j < lambertArr.length(); j++)
					{
						//Get dependency Node of incomming connection aka lambertNode
						MFnDependencyNode lambertNode(lambertArr[j].node());
						MFnLambertShader lambert(lambertArr[j].node());

						//get data
						name = lambert.name().asChar();
						savedColor = lambert.color();
						savedAmbient = lambert.ambientColor();


						// -------- Get color texture filename -------- //

						//Find Color plug
						MPlug colorPlug = lambertNode.findPlug("color", &status);

						//Get incomming connection
						MPlugArray file2Arr;
						colorPlug.connectedTo(file2Arr, true, false, &status);

						// if connected to file
						for (int k = 0; k < file2Arr.length(); k++)
						{
							//Get dependency Node of File Node
							MFnDependencyNode file2Node(file2Arr[k].node());

							//Get plug with the file path
							MPlug pathPlug = file2Node.findPlug("fileTextureName", &status);
							if (status == MS::kSuccess)
							{
								MString tstring;
								pathPlug.getValue(tstring);
								filepath = tstring.asChar();
							}
						}
					}
				}
			}


			//Build struct
			material mat;
			mat.ambientColor[0] = savedAmbient.r;
			mat.ambientColor[1] = savedAmbient.g;
			mat.ambientColor[2] = savedAmbient.b;
			mat.color[0] = savedColor.r;
			mat.color[1] = savedColor.g;
			mat.color[2] = savedColor.b;
			strcpy_s(mat.filepath, filepath.c_str());
			strcpy_s(mat.normalmapfilepath, normalfilepath.c_str());
			strcpy_s(mat.name, name.c_str());


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
		}

	}
		
}