
#ifndef _GUI_GUI_EXTC_HPP_
#define _GUI_GUI_EXTC_HPP_

#include <windows.h>


#define EXTCPROG_WINDOW_WIDTH  400
#define EXTCPROG_WINDOW_HEIGHT 220

// 700-799
#define EPM_STARTEXTRACT   0x0700
#define EPM_UPDATEPROGRESS 0x0701

HINSTANCE _G_GUI_GUI_Extc_hpp_H_Instance;

struct ExtractionInfos
{
    LPTSTR strFrom;
    LPTSTR strTo;

    bool blKeepOriginal;


    long* npProgress;
    int * npExtractionStage;
};

bool StartExtractionWithGUI(ExtractionInfos* pExtcInfo);

#include "GUI-extc.ipp"


#undef EPM_STARTEXTRACT
#undef EPM_UPDATEPROGRESS

#endif
