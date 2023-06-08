#include "maya_includes.h"
#include <maya/MTimer.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <unordered_map>
#include <Windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

#include "Console.h"

#include "Callbacks/CallbackHandler.h"

#include "Callbacks/Timer.h"
#include "Callbacks/NodeAdded.h"
#include "Callbacks/NodeRemoved.h"

#include "IterateDependencyNodes.h"
#include "IterateDagNodes.h"



using namespace std;

/*
 * Plugin entry point
 * For remote control of maya
 * open command port: commandPort -nr -name ":1234"
 * close command port: commandPort -cl -name ":1234"
 * send command: see loadPlugin.py and unloadPlugin.py
 */
 // commandPort -n ":1234"


EXPORT MStatus initializePlugin(MObject obj)
{
	MStatus res = MS::kSuccess;

	MFnPlugin myPlugin(obj, "level editor", "1.0", "Any", &res);
	if (MFAIL(res)) 
	{
		CHECK_MSTATUS(res);
		return res;
	}

	//Console::MayaOutputWindow();
	Console::Activate();

	cout << "Plugin loaded ===========================" << endl;
	Writer::Init();
	IterateDependencyNodes();
	IterateDagNodes();

	//MStringArray eventNames;
	//MEventMessage::getEventNames(eventNames);
	//for (auto& strr: eventNames)
	//{
	//	cout << strr << endl; 
	//}

	

	//CallbackHandler::idArr.append(MCommandMessage::addCommandCallback(ggg));
	CallbackHandler::idArr.append(MDGMessage::addNodeAddedCallback(NodeAdded, "dependNode", NULL));
	CallbackHandler::idArr.append(MDGMessage::addNodeRemovedCallback(NodeRemoved, "dependNode", NULL));
	//CallbackHandler::idArr.append(MTimerMessage::addTimerCallback(5.0f, Timer, NULL));

	CallbackHandler::idArr.append(MUiMessage::add3dViewPreRenderMsgCallback("modelPanel1", cameraMoved));
	CallbackHandler::idArr.append(MUiMessage::add3dViewPreRenderMsgCallback("modelPanel2", cameraMoved));
	CallbackHandler::idArr.append(MUiMessage::add3dViewPreRenderMsgCallback("modelPanel3", cameraMoved));
	CallbackHandler::idArr.append(MUiMessage::add3dViewPreRenderMsgCallback("modelPanel4", cameraMoved));

	return res;
}

EXPORT MStatus uninitializePlugin(MObject obj) 
{

	MFnPlugin plugin(obj);

	cout << "Plugin unloaded =========================" << endl;

	CallbackHandler::Clear();
	Console::Destroy();

	return MS::kSuccess;
}