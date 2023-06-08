#pragma once

void IterateDependencyNodes()
{
	MItDependencyNodes sceneDependencyIterator = MItDependencyNodes(MFn::kInvalid);

	while (!sceneDependencyIterator.isDone())
	{
		MObject node = sceneDependencyIterator.thisNode();
		//cout << "found MFn DependencyNode: " << MFnDependencyNode(node).name() << std::endl;
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

		sceneDependencyIterator.next();
	}
}