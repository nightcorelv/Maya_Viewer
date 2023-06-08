#pragma once

#include "NodeNameChanged.h"

#include "TransformAdded.h"
#include "TranformChanged.h"

#include "MeshAdded.h"
#include "MeshChanged.h"

#include "LambertAdded.h"
#include "LambertChanged.h"

#include "FileTextureAdded.h"
#include "FileTextureChanged.h"

#include "PointLightAdded.h"
#include "PointLightChanged.h"

#include "CameraAdded.h"
#include "CameraChanged.h"


void NodeAdded(MObject& node, void* clientData)
{
	if (node.hasFn(MFn::kDagNode))
	{
		//cout << "MFnDagNode added with node: " << dag.name() << std::endl;
		//CallbackHandler::idArr.append(MNodeMessage::addNameChangedCallback(node, NodeRenamed));

		if (node.hasFn(MFn::kTransform))
		{
			TransformAdded(node);
			CallbackHandler::AddNameChangeCallback(node, NodeNameChanged);
			CallbackHandler::AddChangedCallback(node, TranformChanged);
		}
		else if (node.hasFn(MFn::kMesh))
		{
			MeshAdded(node);
			CallbackHandler::AddNameChangeCallback(node, NodeNameChanged);
			CallbackHandler::AddChangedCallback(node, MeshChanged);
		}
		else if (node.hasFn(MFn::kCamera))
		{
			CameraAdded(node);
			CallbackHandler::AddNameChangeCallback(node, NodeNameChanged);
			CallbackHandler::AddChangedCallback(node, CameraChanged);
		}
	}
	else
	{

		if (node.hasFn(MFn::kLambert))
		{
			LambertAdded(node);
			CallbackHandler::AddNameChangeCallback(node, NodeNameChanged);
			CallbackHandler::AddChangedCallback(node, LambertChanged);
		}
		else if (node.hasFn(MFn::kFileTexture))
		{
			FileTextureAdded(node);
			CallbackHandler::AddNameChangeCallback(node, NodeNameChanged);
			CallbackHandler::AddChangedCallback(node, FileTextureChanged);
		}

	}

}