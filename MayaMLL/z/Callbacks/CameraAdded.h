#pragma once
#include "../SharedMemory/Writer.h"

void CameraAdded(MObject& node)
{
	MFnCamera camera(node);

	//Assemble data
	cameraAddStruct cam;

	std::string camName = camera.name().asChar();
	camName.erase(camName.find("Shape"), 5);

	strcpy_s(cam.name, camName.c_str());
	cam.aspect = (float)camera.aspectRatio();
	cam.fov = (float)camera.horizontalFieldOfView() * 57.2957795;
	cam.zFar = (float)camera.farClippingPlane();
	cam.zNear = (float)camera.nearClippingPlane();
	cam.isOrtho = camera.isOrtho();
	cam.orthoWidth = (float)camera.orthoWidth();
	cam.orthoHeight = (float)camera.orthoWidth() / (float)camera.aspectRatio();

	MMatrix m = MDagPath::getAPathTo(node).inclusiveMatrix();

	cam.m[0] = (float)m.matrix[0][0];
	cam.m[1] = (float)m.matrix[0][1];
	cam.m[2] = (float)m.matrix[0][2];
	cam.m[3] = (float)m.matrix[0][3];
	cam.m[4] = (float)m.matrix[1][0];
	cam.m[5] = (float)m.matrix[1][1];
	cam.m[6] = (float)m.matrix[1][2];
	cam.m[7] = (float)m.matrix[1][3];
	cam.m[8] = (float)m.matrix[2][0];
	cam.m[9] = (float)m.matrix[2][1];
	cam.m[10] = (float)m.matrix[2][2];
	cam.m[11] = (float)m.matrix[2][3];
	cam.m[12] = (float)m.matrix[3][0];
	cam.m[13] = (float)m.matrix[3][1];
	cam.m[14] = (float)m.matrix[3][2];
	cam.m[15] = (float)m.matrix[3][3];
	
	//Send
	StructHeader header;
	header.dataSize = sizeof(cameraAddStruct);
	header.id = DataType::CamAdded;
	Writer::Send(header, &cam);

	//Print
	cout << "Camera Name: " << camera.name() << endl;
	cout << "Camera Znear: " << camera.nearClippingPlane() << endl;
	cout << "Camera Zfar: " << camera.farClippingPlane() << endl;
	cout << "Camera Aspect ratio: " << camera.aspectRatio() << endl;
	cout << "Camera FOV: " << (camera.horizontalFieldOfView() * 57.2957795) << endl;
	cout << "Camera isOrtho: " << camera.isOrtho() << endl;
	cout << "Camera orthoWidth: " << camera.orthoWidth() << endl;
	cout << "Camera orthoHeight " << camera.orthoWidth() / camera.aspectRatio() << endl;
	cout << endl;

}