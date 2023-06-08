#pragma once

enum DataType
{
	TransformChanged,
	MeshAdd,
	MeshHeader,
	Vertex,
	MaterialChanged,
	MaterialSwapped,
	CamAdded,
	CamChanged,
	NameChanged,
	NodeRemove,
	MeshEnum,
	MaterialEnum,
	CameraActivate
};

//---------------- Struct header ----------------
struct StructHeader
{
	size_t dataSize = 0;
	int id = 0;
};


//---------- Material changed callback ----------

struct material
{
	char name[64];
	char filepath[256];
	char normalmapfilepath[256];
	float color[3];
	float ambientColor[3];
};

struct swappedMaterial
{
	char meshName[64];
	char newName[64];
	//char oldName[64];
};

//---------- Transform changed callback ----------

struct worldMatrix
{
	char name[64];
	float m[16];
};

//---------- Mesh changed callback ----------

struct MeshAddedHeader
{
	char name[64];
	char materialName[64];
	int nrOfVert;
	float m[16];
};


struct meshHeader
{
	char name[64];
	char materialName[64];
	int nrOfVert;
};

struct vertex
{
	float position[3];
	float uv[2];
	float normal[3];
};

//-------------- Camera changed ---------------

struct cameraActivated
{
	char name[32];
};

struct cameraStruct
{
	char name[32];
	float zNear;
	float zFar;
	float aspect;
	float fov;
};

struct cameraAddStruct
{
	char name[32];
	float zNear;
	float zFar;
	float aspect;
	float fov;
	float orthoWidth;
	float orthoHeight;
	bool isOrtho;
	float m[16];
};

//--------------- Name Changed --------------

struct namesChanged
{
	char name[64];
	char previousName[64];
};

//---------------- Node removed -------------

struct removedStruct
{
	char name[64];
	int id;
};