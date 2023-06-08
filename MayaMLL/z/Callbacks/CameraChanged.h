#pragma once
#include "../SharedMemory/Writer.h"

void CameraChanged(MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* x)
{

	if (msg & MNodeMessage::AttributeMessage::kAttributeSet)
	{

	}


}
std::string preActive = "";
//Called pre-render for every panel, make sure camera update only occurs for the active panel
void cameraMoved(const MString& str, void* clientData) 
{
    MString cmd = "getPanel -wf"; //wf = with focus (current panel)
    MString activePanel = MGlobal::executeCommandStringResult(cmd);

    if (std::string(str.asChar()) == std::string(activePanel.asChar())) 
    {
        M3dView currentView = M3dView::active3dView();
        currentView.updateViewingParameters();

        MDagPath dagPath;
        currentView.getCamera(dagPath);

        if (preActive != dagPath.partialPathName().asChar())
        {
            preActive = dagPath.partialPathName().asChar();
            cout << "active cam is : " << preActive << endl;

            std::string camName = dagPath.partialPathName().asChar();
            int sindex = -1;
            sindex = camName.find("Shape");
            if (sindex != -1)
            {
                camName.erase(sindex, 5);
            }
            cout << "active cam is : " << camName << endl;
            cameraActivated cam;
            strcpy_s(cam.name, camName.c_str());

            //Send
            StructHeader header;
            header.dataSize = sizeof(cameraActivated);
            header.id = DataType::CameraActivate;
            Writer::Send(header, &cam);

            cout << endl;
        }
    }
}