
#ifndef _GUI_GUI_EXTC_HPP_
#define _GUI_GUI_EXTC_HPP_

#include <windows.h>


#define EXTCPROG_WINDOW_WIDTH  400
#define EXTCPROG_WINDOW_HEIGHT 220

HINSTANCE _G_GUI_GUI_Extc_hpp_H_Instance;

struct ExtractionInfos
{
    LPTSTR strFrom;
    LPTSTR strTo;

    bool blKeepOriginal;


    long* npProgress;
    int * npExtractionStage;
};

bool StartExtractionWithGUI(ExtractionInfos* pExtcInfo, HWND hParentWindow);

#include "GUI-extc.ipp"


#endif
