#include "MayaViewer.h"
#include "Reader.h"
#include "Structs.h"

#include <unordered_map>
#include <string>


// Declare our game instance
MayaViewer game;

constexpr int gModelCount = 10;

// material name, material property
std::unordered_map<std::string, material> materials;

// mesh name, material name
std::unordered_map<std::string, std::string> meshMatKey;

// mesh name, world matrix
std::unordered_map<std::string, Matrix> meshransform;

// mesh name, model*
std::unordered_map<std::string, Model*> models;

// mesh name, vertices data
std::unordered_map<std::string, std::vector<vertex>> meshVertices;

Node* lightNode;

static bool gKeys[256] = {};
int gDeltaX;
int gDeltaY;
bool gMousePressed;

Scene* g_scene;
//Texture::Sampler* sampler_t;

MayaViewer::MayaViewer()
    : _scene(NULL), _wireframe(false)
{


}

Node* CreatePerspectiveCamera(std::string name, Vector3 position, Quaternion roatation,Vector3 scale, float fov, float znear, float zfar, float aspectRatio)
{
	Camera* cam = Camera::createPerspective(fov, aspectRatio, znear, zfar);
	
	Node* cameraNode = g_scene->addNode(name.c_str());
	cameraNode->setCamera(cam);
	g_scene->setActiveCamera(cam);
	SAFE_RELEASE(cam);

	cameraNode->translate(position);
	//cameraNode->rotateX(MATH_DEG_TO_RAD(0.f));
	cameraNode->setRotation(roatation);
	cameraNode->scale(scale);

	return cameraNode;
}

Node* CreateOrthographicCamera(std::string name, Vector3 position, Quaternion roatation, Vector3 scale, float zoomX, float zoomY, float aspectRatio, float znear, float zfar)
{
	Camera* cam = Camera::createOrthographic(zoomX, zoomY, aspectRatio, znear, zfar);

	Node* cameraNode = g_scene->addNode(name.c_str());
	cameraNode->setCamera(cam);
	g_scene->setActiveCamera(cam);
	SAFE_RELEASE(cam);

	cameraNode->translate(position);
	//cameraNode->rotateX(MATH_DEG_TO_RAD(0.f));
	cameraNode->setRotation(roatation);
	cameraNode->setScale(scale);

	return cameraNode;
}

void SwitchCam(std::string name)
{
	Node* cameraNode = g_scene->findNode(name.c_str());
	Camera* cam = cameraNode->getCamera();
	cameraNode->setCamera(cam);
	g_scene->setActiveCamera(cam);
}

Node* MayaViewer::CreatePointLight(std::string name, Vector3 position, Vector3 color, float range)
{
	Node* lightNode = _scene->addNode(name.c_str());
	Light* light = Light::createPoint(color, range);
	lightNode->setLight(light);
	SAFE_RELEASE(light);
	lightNode->translate(position);

	return lightNode;
}

Mesh* CreateMesh2(std::vector<vertex>& verts)
{
	std::vector<float> vertexBuffer;
	for (int i = 0; i < verts.size(); i++)
	{
		vertexBuffer.emplace_back(verts[i].position[0]);
		vertexBuffer.emplace_back(verts[i].position[1]);
		vertexBuffer.emplace_back(verts[i].position[2]);

		vertexBuffer.emplace_back(verts[i].uv[0]);
		vertexBuffer.emplace_back(verts[i].uv[1]);

		vertexBuffer.emplace_back(verts[i].normal[0]);
		vertexBuffer.emplace_back(verts[i].normal[1]);
		vertexBuffer.emplace_back(verts[i].normal[2]);
	}

	//input layout
	VertexFormat::Element elements[] = {
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::TEXCOORD0, 2),
		VertexFormat::Element(VertexFormat::NORMAL, 3),
	};

	Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 3), verts.size(), true);
	if (mesh == NULL)
	{
		GP_ERROR("Failed to create stupid mesh");
		return NULL;
	}

	mesh->setVertexData(vertexBuffer.data(), 0, verts.size());

	return mesh;
}

void SetAmbientColor(Material* mat, Vector3 value)
{
	mat->getParameter("u_ambientColor")->setValue(value);
}

void SetDiffuseTexture(Material* mat, std::string path)
{
	Texture::Sampler* sampler_t = mat->getParameter("u_diffuseTexture")->setValue("res/png/crate.png", true);
	sampler_t->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);
}

void SetNormalTexture(Material* mat, std::string path)
{
	Texture::Sampler* sampler_t = mat->getParameter("u_normalmapTexture")->setValue("res/png/crate.png", true);
	sampler_t->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);
}

void MayaViewer::initialize()
{
	Reader::Init();

    // Load game scene from file
	_scene = Scene::create();
	g_scene = _scene;

	lightNode = CreatePointLight("light");

	while (!Reader::AllDataIsReaded())
	{
		StructHeader header;
		Reader::ReadHeader(header);

		switch (header.id)
		{

		case TransformChanged: {
			worldMatrix data;
			Reader::ReadData(header, &data);
			Matrix m;
			for (int i = 0; i < 16; i++)
			{
				m.m[i] = data.m[i];
			}
			meshransform[data.name] = m;

		}break;

		case MaterialChanged: {
			material data;
			Reader::ReadData(header, &data);
			materials[data.name] = data;
		}break;

		case MaterialSwapped: {
			swappedMaterial data;
			Reader::ReadData(header, &data);
			meshMatKey[data.meshName] = data.newName;
		}break;

		case MeshHeader: {
			meshHeader mh;
			Reader::ReadData(header, &mh);
			meshMatKey[mh.name] = mh.materialName;

			StructHeader vertexHeader;
			Reader::ReadHeader(vertexHeader);

			std::vector<vertex> vertices;
			vertices.resize(mh.nrOfVert);
			Reader::ReadData(vertexHeader, &vertices[0]);
			meshVertices[mh.name] = vertices;

		}break;

		case MeshAdd: {
			MeshAddedHeader mh;
			Reader::ReadData(header, &mh);
			meshMatKey[mh.name] = mh.materialName;

			StructHeader vertexHeader;
			Reader::ReadHeader(vertexHeader);

			std::vector<vertex> vertices;
			vertices.resize(mh.nrOfVert);
			Reader::ReadData(vertexHeader, &vertices[0]);
			meshVertices[mh.name] = vertices;

			Matrix m;
			for (int i = 0; i < 16; i++)
			{
				m.m[i] = mh.m[i];
			}
			meshransform[mh.name] = m;

		}break;

		case CamAdded: {
			cameraAddStruct data;
			Reader::ReadData(header, &data);

			Matrix m;
			for (int i = 0; i < 16; i++)
			{
				m.m[i] = data.m[i];
			}
			Vector3 translation, scale;
			Quaternion rotation;
			m.decompose(&scale, &rotation, &translation);

			if (data.isOrtho)
			{
				CreateOrthographicCamera(data.name, translation, rotation, scale, data.orthoWidth, data.orthoHeight, data.aspect, data.zNear, data.zFar);
			}
			else
			{
				CreatePerspectiveCamera(data.name, translation, rotation, scale, data.fov, data.zNear, data.zFar, data.aspect);
			}

		}break;

		case CameraActivate: {
			cameraActivated data;
			Reader::ReadData(header, &data);
			SwitchCam(data.name);
		}break;

		case CamChanged: {

		}break;

		case NameChanged: {
			namesChanged data;
			Reader::ReadData(header, &data);

			if (Node* node = _scene->findNode(data.previousName))
			{
				meshransform.insert({ data.name , meshransform[data.previousName] });
				meshMatKey.insert({ data.name , meshMatKey[data.previousName] });
				meshVertices.insert({ data.name , meshVertices[data.previousName] });

				meshransform.erase(data.name);
				meshMatKey.erase(data.name);
				meshVertices.erase(data.name);
			}
		}break;

		case NodeRemove: {
			removedStruct data;
			Reader::ReadData(header, &data);
			if (data.id == MeshEnum)
			{
				meshransform.erase(data.name);
				meshMatKey.erase(data.name);
				meshVertices.erase(data.name);
			}
			else if (data.id == MaterialEnum)
			{
				// todo
			}

		}break;

		}
	}

	SwitchCam("persp");

	for (auto& verArr : meshVertices)
	{
		Mesh* mesh = CreateMesh2(verArr.second);
		Model* model = Model::create(mesh);
		Material* mat;
		Texture::Sampler* sampler;

		if (!std::string(materials[meshMatKey[verArr.first]].filepath).empty())
		{
			mat = model->setMaterial("res/shaders/textured.vert", "res/shaders/textured.frag", "POINT_LIGHT_COUNT 1");
			sampler = mat->getParameter("u_diffuseTexture")->setValue(materials[meshMatKey[verArr.first]].filepath, true);
			sampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);

			if (!std::string(materials[meshMatKey[verArr.first]].normalmapfilepath).empty())
			{
				sampler = mat->getParameter("u_normalmapTexture")->setValue(materials[meshMatKey[verArr.first]].normalmapfilepath, true);
				sampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);
			}
		}
		else
		{
			mat = model->setMaterial("res/shaders/colored.vert", "res/shaders/colored.frag", "POINT_LIGHT_COUNT 1");
			mat->getParameter("u_diffuseColor")->setValue(Vector4(materials[meshMatKey[verArr.first]].color[0], materials[meshMatKey[verArr.first]].color[1], materials[meshMatKey[verArr.first]].color[2], 1));
		}
		mat->getParameter("u_ambientColor")->setValue(Vector3(materials[meshMatKey[verArr.first]].ambientColor[0], materials[meshMatKey[verArr.first]].ambientColor[1], materials[meshMatKey[verArr.first]].ambientColor[2]));

		mat->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
		mat->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");

		//light
		mat->getParameter("u_pointLightColor[0]")->setValue(lightNode->getLight()->getColor());
		mat->getParameter("u_pointLightPosition[0]")->bindValue(lightNode, &Node::getTranslationWorld);
		mat->getParameter("u_pointLightRangeInverse[0]")->bindValue(lightNode->getLight(), &Light::getRangeInverse);

		mat->getStateBlock()->setCullFace(true);
		mat->getStateBlock()->setDepthTest(true);
		mat->getStateBlock()->setDepthWrite(true);

		Node* node = _scene->addNode(verArr.first.c_str());

		Vector3 translation, scale;
		Quaternion rotation; 
		meshransform[verArr.first].decompose(&scale, &rotation, &translation);

		node->setTranslation(translation);
		node->setRotation(rotation);
		node->setScale(scale);

		node->setDrawable(model);
		SAFE_RELEASE(model);
	}

}

void MayaViewer::finalize()
{
    SAFE_RELEASE(_scene);
}

void MayaViewer::update(float elapsedTime)
{

	while (!Reader::AllDataIsReaded())
	{
		StructHeader header;
		Reader::ReadHeader(header);

		switch (header.id)
		{

		case TransformChanged: {
			worldMatrix data;
			Reader::ReadData(header, &data);
			Matrix m;
			for (int i = 0; i < 16; i++)
			{
				m.m[i] = data.m[i];
			}
			meshransform[data.name] = m;

			Vector3 translation, scale;
			Quaternion rotation;
			m.decompose(&scale, &rotation, &translation);

			if (Node* node = g_scene->findNode(data.name))
			{
				node->setTranslation(translation);
				node->setRotation(rotation);
				node->setScale(scale);
			}

		}break;

		case MaterialChanged: {
			material data;
			Reader::ReadData(header, &data);
			
			materials[data.name] = data;

			for (auto& meshMat : meshMatKey)
			{
				if (meshMat.second == data.name)
				{

					if (Node* noode = _scene->findNode(meshMat.first.c_str()))
					{
						_scene->removeNode(noode);
					}

					Mesh* mesh = CreateMesh2(meshVertices[meshMat.first]);
					Model* model;
					Material* mat;
					Texture::Sampler* sampler;

					model = Model::create(mesh);

					if (!std::string(materials[meshMat.second].filepath).empty())
					{
						mat = model->setMaterial("res/shaders/textured.vert", "res/shaders/textured.frag", "POINT_LIGHT_COUNT 1");
						sampler = mat->getParameter("u_diffuseTexture")->setValue(materials[meshMat.second].filepath, true);
						sampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);

						if (!std::string(materials[meshMat.second].normalmapfilepath).empty())
						{
							sampler = mat->getParameter("u_normalmapTexture")->setValue(materials[meshMat.second].normalmapfilepath, true);
							sampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);
						}
					}
					else
					{
						mat = model->setMaterial("res/shaders/colored.vert", "res/shaders/colored.frag", "POINT_LIGHT_COUNT 1");
						mat->getParameter("u_diffuseColor")->setValue(Vector4(materials[meshMat.second].color[0], materials[meshMat.second].color[1], materials[meshMat.second].color[2], 1));
					}
					mat->getParameter("u_ambientColor")->setValue(Vector3(materials[meshMat.second].ambientColor[0], materials[meshMat.second].ambientColor[1], materials[meshMat.second].ambientColor[2]));

					mat->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
					mat->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");

					//light
					mat->getParameter("u_pointLightColor[0]")->setValue(lightNode->getLight()->getColor());
					mat->getParameter("u_pointLightPosition[0]")->bindValue(lightNode, &Node::getTranslationWorld);
					mat->getParameter("u_pointLightRangeInverse[0]")->bindValue(lightNode->getLight(), &Light::getRangeInverse);

					mat->getStateBlock()->setCullFace(true);
					mat->getStateBlock()->setDepthTest(true);
					mat->getStateBlock()->setDepthWrite(true);



					Node* node = _scene->addNode(meshMat.first.c_str());

					Vector3 translation, scale;
					Quaternion rotation;
					meshransform[meshMat.first].decompose(&scale, &rotation, &translation);

					node->setTranslation(translation);
					node->setRotation(rotation);
					node->setScale(scale);

					node->setDrawable(model);
					SAFE_RELEASE(model);
				}

			}

		}break;

		case MaterialSwapped: {
			swappedMaterial data;
			Reader::ReadData(header, &data);
			meshMatKey[data.meshName] = data.newName;

			Mesh* mesh = CreateMesh2(meshVertices[data.meshName]);
			Model* model = Model::create(mesh);
			Material* mat;
			Texture::Sampler* sampler;

			if (!std::string(materials[meshMatKey[data.meshName]].filepath).empty())
			{
				mat = model->setMaterial("res/shaders/textured.vert", "res/shaders/textured.frag", "POINT_LIGHT_COUNT 1");
				sampler = mat->getParameter("u_diffuseTexture")->setValue(materials[meshMatKey[data.meshName]].filepath, true);
				sampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);

				if (!std::string(materials[meshMatKey[data.meshName]].normalmapfilepath).empty())
				{
					sampler = mat->getParameter("u_normalmapTexture")->setValue(materials[meshMatKey[data.meshName]].normalmapfilepath, true);
					sampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);
				}
			}
			else
			{
				mat = model->setMaterial("res/shaders/colored.vert", "res/shaders/colored.frag", "POINT_LIGHT_COUNT 1");
				mat->getParameter("u_diffuseColor")->setValue(Vector4(materials[meshMatKey[data.meshName]].color[0], materials[meshMatKey[data.meshName]].color[1], materials[meshMatKey[data.meshName]].color[2], 1));
			}
			mat->getParameter("u_ambientColor")->setValue(Vector3(materials[meshMatKey[data.meshName]].ambientColor[0], materials[meshMatKey[data.meshName]].ambientColor[1], materials[meshMatKey[data.meshName]].ambientColor[2]));

			mat->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
			mat->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");

			Node* lightNode = g_scene->findNode("light");

			//light
			mat->getParameter("u_pointLightColor[0]")->setValue(lightNode->getLight()->getColor());
			mat->getParameter("u_pointLightPosition[0]")->bindValue(lightNode, &Node::getTranslationWorld);
			mat->getParameter("u_pointLightRangeInverse[0]")->bindValue(lightNode->getLight(), &Light::getRangeInverse);

			mat->getStateBlock()->setCullFace(true);
			mat->getStateBlock()->setDepthTest(true);
			mat->getStateBlock()->setDepthWrite(true);

			Node* noode = _scene->findNode(data.meshName);
			if (noode != nullptr)
			{
				_scene->removeNode(noode);
			}

			Node* node = _scene->addNode(data.meshName);

			Vector3 translation, scale;
			Quaternion rotation;
			meshransform[data.meshName].decompose(&scale, &rotation, &translation);

			node->translate(translation);
			node->setRotation(rotation);
			node->setScale(scale);

			node->setDrawable(model);
			SAFE_RELEASE(model);

		}break;

		case MeshHeader: {
			meshHeader mh;
			Reader::ReadData(header, &mh);
			meshMatKey[mh.name] = mh.materialName;

			StructHeader vertexHeader;
			Reader::ReadHeader(vertexHeader);

			std::vector<vertex> vertices;
			vertices.resize(mh.nrOfVert);
			Reader::ReadData(vertexHeader, &vertices[0]);
			meshVertices[mh.name] = vertices;

			Mesh* mesh = CreateMesh2(vertices);
			Model* model;
			Material* mat;
			Texture::Sampler* sampler;

			model = Model::create(mesh);

			if (!std::string(materials[meshMatKey[mh.name]].filepath).empty())
			{
				mat = model->setMaterial("res/shaders/textured.vert", "res/shaders/textured.frag", "POINT_LIGHT_COUNT 1");
				sampler = mat->getParameter("u_diffuseTexture")->setValue(materials[meshMatKey[mh.name]].filepath, true);
				sampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);

				if (!std::string(materials[meshMatKey[mh.name]].normalmapfilepath).empty())
				{
					sampler = mat->getParameter("u_normalmapTexture")->setValue(materials[meshMatKey[mh.name]].normalmapfilepath, true);
					sampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);
				}
			}
			else
			{
				mat = model->setMaterial("res/shaders/colored.vert", "res/shaders/colored.frag", "POINT_LIGHT_COUNT 1");
				mat->getParameter("u_diffuseColor")->setValue(Vector4(materials[meshMatKey[mh.name]].color[0], materials[meshMatKey[mh.name]].color[1], materials[meshMatKey[mh.name]].color[2], 1));
			}
			mat->getParameter("u_ambientColor")->setValue(Vector3(materials[meshMatKey[mh.name]].ambientColor[0], materials[meshMatKey[mh.name]].ambientColor[1], materials[meshMatKey[mh.name]].ambientColor[2]));

			mat->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
			mat->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");

			Node* lightNode = g_scene->findNode("light");

			//light
			mat->getParameter("u_pointLightColor[0]")->setValue(lightNode->getLight()->getColor());
			mat->getParameter("u_pointLightPosition[0]")->bindValue(lightNode, &Node::getTranslationWorld);
			mat->getParameter("u_pointLightRangeInverse[0]")->bindValue(lightNode->getLight(), &Light::getRangeInverse);

			mat->getStateBlock()->setCullFace(true);
			mat->getStateBlock()->setDepthTest(true);
			mat->getStateBlock()->setDepthWrite(true);

			if (Node* noode = _scene->findNode(mh.name))
			{
				_scene->removeNode(noode);
			}

			Node* node = _scene->addNode(mh.name);

			Vector3 translation, scale;
			Quaternion rotation;
			meshransform[mh.name].decompose(&scale, &rotation, &translation);

			node->translate(translation);
			node->setRotation(rotation);
			node->setScale(scale);

			node->setDrawable(model);
			SAFE_RELEASE(model);

		}break;

		case MeshAdd: {
			MeshAddedHeader mh;
			Reader::ReadData(header, &mh);
			meshMatKey[mh.name] = mh.materialName;

			StructHeader vertexHeader;
			Reader::ReadHeader(vertexHeader);

			std::vector<vertex> vertices;
			vertices.resize(mh.nrOfVert);
			Reader::ReadData(vertexHeader, &vertices[0]);

			meshVertices[mh.name] = vertices;

			Matrix m;
			for (int i = 0; i < 16; i++)
			{
				m.m[i] = mh.m[i];
			}

			meshransform[mh.name] = m;

		}break;

		case CamAdded: {
			cameraAddStruct data;
			Reader::ReadData(header, &data);

			Matrix m;

			for (int i = 0; i < 16; i++)
			{
				m.m[i] = data.m[i];
			}

			Vector3 translation, scale;
			Quaternion rotation;
			m.decompose(&scale, &rotation, &translation);

			if (data.isOrtho)
			{
				CreateOrthographicCamera(data.name, translation, rotation, scale, data.orthoWidth, data.orthoHeight, data.aspect, data.zNear, data.zFar);
			}
			else
			{
				CreatePerspectiveCamera(data.name, translation, rotation, scale, data.fov, data.zNear, data.zFar, data.aspect);
			}

		}break;

		case CameraActivate: {
			cameraActivated data;
			Reader::ReadData(header, &data);
			SwitchCam(data.name);
		}break;

		case CamChanged: {

		}break;

		case NameChanged: {
			namesChanged data;
			Reader::ReadData(header, &data);

			if (Node* node = _scene->findNode(data.previousName))
			{
				meshransform.insert({ data.name , meshransform[data.previousName] });
				meshMatKey.insert({ data.name , meshMatKey[data.previousName] });
				meshVertices.insert({ data.name , meshVertices[data.previousName] });
				
				meshransform.erase(data.name);
				meshMatKey.erase(data.name);
				meshVertices.erase(data.name);

				node->setId(data.name);
			}

		}break;

		case NodeRemove: {
			removedStruct data;
			Reader::ReadData(header, &data);
			if (data.id == MeshEnum)
			{
				if (Node* node = _scene->findNode(data.name))
				{
					meshransform.erase(data.name);
					meshMatKey.erase(data.name);
					meshVertices.erase(data.name);
					_scene->removeNode(node);
				}

			}
			else if (data.id == MaterialEnum)
			{
				// todo
			}

		}break;

		}
	}

	//Node* camnode = _scene->getActiveCamera()->getNode();
	//if (gKeys[Keyboard::KEY_W])
	//	camnode->translateForward(0.5);
	//if (gKeys[Keyboard::KEY_S])
	//	camnode->translateForward(-0.5);
	//if (gKeys[Keyboard::KEY_A])
	//	camnode->translateLeft(0.5);
	//if (gKeys[Keyboard::KEY_D])
	//	camnode->translateLeft(-0.5);
	//
	//if (gMousePressed) {
	//	camnode->rotate(camnode->getRightVectorWorld(), MATH_DEG_TO_RAD(gDeltaY / 10.0));
	//	camnode->rotate(camnode->getUpVectorWorld(), MATH_DEG_TO_RAD(gDeltaX / 5.0));
	//}

}

void MayaViewer::render(float elapsedTime)
{
    // Clear the color and depth buffers
    clear(CLEAR_COLOR_DEPTH, Vector4(0.1f,0.0f,0.0f,1.0f), 1.0f, 0);

    // Visit all the nodes in the scene for drawing
    _scene->visit(this, &MayaViewer::drawScene);
}

bool MayaViewer::drawScene(Node* node)
{

    // If the node visited contains a drawable object, draw it
    Drawable* drawable = node->getDrawable(); 
    if (drawable)
        drawable->draw(_wireframe);

    return true;
}

void MayaViewer::keyEvent(Keyboard::KeyEvent evt, int key)
{
    if (evt == Keyboard::KEY_PRESS)
    {
		gKeys[key] = true;
        switch (key)
        {
        case Keyboard::KEY_ESCAPE:
            exit();
            break;
		};
    }
	else if (evt == Keyboard::KEY_RELEASE)
	{
		gKeys[key] = false;
	}
}

bool MayaViewer::mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
	static int lastX = 0;
	static int lastY = 0;
	gDeltaX = lastX - x;
	gDeltaY = lastY - y;
	lastX = x;
	lastY = y;
	gMousePressed =
		(evt == Mouse::MouseEvent::MOUSE_PRESS_LEFT_BUTTON) ? true :
		(evt == Mouse::MouseEvent::MOUSE_RELEASE_LEFT_BUTTON) ? false : gMousePressed;

	return true;
}

void MayaViewer::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    switch (evt)
    {
    case Touch::TOUCH_PRESS:
        _wireframe = !_wireframe;
        break;
    case Touch::TOUCH_RELEASE:
        break;
    case Touch::TOUCH_MOVE:
        break;
    };
}

Mesh* MayaViewer::createCubeMesh(float size)
{
	float a = size * 0.5f;
	float vertices[] =
	{
		-a, -a,  a,    0.0,  0.0,  1.0,   0.0, 0.0,
		a, -a,  a,    0.0,  0.0,  1.0,   1.0, 0.0,
		-a,  a,  a,    0.0,  0.0,  1.0,   0.0, 1.0,
		a,  a,  a,    0.0,  0.0,  1.0,   1.0, 1.0,
		-a,  a,  a,    0.0,  1.0,  0.0,   0.0, 0.0,
		a,  a,  a,    0.0,  1.0,  0.0,   1.0, 0.0,
		-a,  a, -a,    0.0,  1.0,  0.0,   0.0, 1.0,
		a,  a, -a,    0.0,  1.0,  0.0,   1.0, 1.0,
		-a,  a, -a,    0.0,  0.0, -1.0,   0.0, 0.0,
		a,  a, -a,    0.0,  0.0, -1.0,   1.0, 0.0,
		-a, -a, -a,    0.0,  0.0, -1.0,   0.0, 1.0,
		a, -a, -a,    0.0,  0.0, -1.0,   1.0, 1.0,
		-a, -a, -a,    0.0, -1.0,  0.0,   0.0, 0.0,
		a, -a, -a,    0.0, -1.0,  0.0,   1.0, 0.0,
		-a, -a,  a,    0.0, -1.0,  0.0,   0.0, 1.0,
		a, -a,  a,    0.0, -1.0,  0.0,   1.0, 1.0,
		a, -a,  a,    1.0,  0.0,  0.0,   0.0, 0.0,
		a, -a, -a,    1.0,  0.0,  0.0,   1.0, 0.0,
		a,  a,  a,    1.0,  0.0,  0.0,   0.0, 1.0,
		a,  a, -a,    1.0,  0.0,  0.0,   1.0, 1.0,
		-a, -a, -a,   -1.0,  0.0,  0.0,   0.0, 0.0,
		-a, -a,  a,   -1.0,  0.0,  0.0,   1.0, 0.0,
		-a,  a, -a,   -1.0,  0.0,  0.0,   0.0, 1.0,
		-a,  a,  a,   -1.0,  0.0,  0.0,   1.0, 1.0
	};
	short indices[] =
	{
		0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 18, 17, 19, 20, 21, 22, 22, 21, 23
	};
	unsigned int vertexCount = 24;
	unsigned int indexCount = 36;
	VertexFormat::Element elements[] =
	{
		VertexFormat::Element(VertexFormat::POSITION, 3),
		VertexFormat::Element(VertexFormat::NORMAL, 3),
		VertexFormat::Element(VertexFormat::TEXCOORD0, 2)
	};
	Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 3), vertexCount, false);
	if (mesh == NULL)
	{
		GP_ERROR("Failed to create mesh.");
		return NULL;
	}
	mesh->setVertexData(vertices, 0, vertexCount);
	MeshPart* meshPart = mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX16, indexCount, false);
	meshPart->setIndexData(indices, 0, indexCount);
	return mesh;
}
