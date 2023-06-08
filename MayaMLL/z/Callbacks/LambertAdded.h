#pragma once

void LambertAdded(MObject& node)
{
	// ------------------------ Send Material ---------------------------
	MStatus status = MS::kSuccess;

	//Variables 
	std::string filepath = "";
	std::string normalfilepath = "";
	MFnLambertShader lambert(node);

	//Find Color plug
	MFnDependencyNode lambertNode(node);
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
	strcpy_s(mat.filepath, filepath.c_str());
	strcpy_s(mat.normalmapfilepath, normalfilepath.c_str());
	strcpy_s(mat.name, lambert.name().asChar());

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