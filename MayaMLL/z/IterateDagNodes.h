#pragma once

void IterateDagNodes()
{
	MItDag sceneDagIterator = MItDag(MItDag::kDepthFirst, MFn::kInvalid);

	while (!sceneDagIterator.isDone())
	{
		MObject node = sceneDagIterator.currentItem();
		//cout << "found MFnDagNode: " << MFnDagNode(node).name() << std::endl;

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

		sceneDagIterator.next();
	}
}