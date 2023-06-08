#pragma once

struct CallbackHandler
{
	inline static MCallbackIdArray idArr;
	inline static std::unordered_map<std::string, MCallbackId> callbacks;


	static void AddNameChangeCallback(MObject& node, MNodeMessage::MNodeStringFunction func)
	{
		CallbackHandler::callbacks[std::string(MFnDependencyNode(node).name().asChar()) + ".nameChanged"] = MNodeMessage::addNameChangedCallback(node, func);
	}

	static void AddChangedCallback(MObject& node, MNodeMessage::MAttr2PlugFunction func)
	{
		CallbackHandler::callbacks[std::string(MFnDependencyNode(node).name().asChar()) + ".changed"] = MNodeMessage::addAttributeChangedCallback(node, func);
	}

	//static void AddNameChangeAndChangedCallback(MObject& node, MNodeMessage::MNodeStringFunction nameFunc, MNodeMessage::MAttr2PlugFunction changedFunc)
	//{
	//	std::string nodeName = MFnDependencyNode(node).name().asChar();
	//	CallbackHandler::callbacks[nodeName + ".nameChanged"] = MNodeMessage::addNameChangedCallback(node, nameFunc);
	//	CallbackHandler::callbacks[nodeName + ".changed"] = MNodeMessage::addAttributeChangedCallback(node, changedFunc);
	//}

	static void RemoveNameChangeCallback(MObject& node)
	{
		std::string callBackName = std::string(MFnDependencyNode(node).name().asChar()) + ".nameChanged";
		MMessage::removeCallback(callbacks[callBackName]);
		CallbackHandler::callbacks.erase(callBackName);
	}

	static void RemoveChangedCallback(MObject& node)
	{
		std::string callBackName = std::string(MFnDependencyNode(node).name().asChar()) + ".changed";
		MMessage::removeCallback(callbacks[callBackName]);
		CallbackHandler::callbacks.erase(callBackName);
	}

	static void Clear()
	{
		for (auto& i : CallbackHandler::callbacks)
		{
			MMessage::removeCallback(i.second);
		}

		for (auto& id : CallbackHandler::idArr)
		{
			MMessage::removeCallback(id);
		}
	}

};