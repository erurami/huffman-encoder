
#ifndef _GUI_GUI_COMP_HPP_
#define _GUI_GUI_COMP_HPP_

#include <windows.h>


#define COMPPROG_WINDOW_WIDTH  400
#define COMPPROG_WINDOW_HEIGHT 220

// 600-699
#define CPM_STARTCOMPRESS    0x0601
#define CPM_UPDATEPROGRESS   0x0602

HINSTANCE _G_GUI_GUI_Comp_hpp_H_Instance;

struct CompressionInfos
{
    LPTSTR strFrom;
    LPTSTR strTo;

    bool blKeepOriginal;


    long* npProgress;
    int * npCompressionStage;
};

bool StartCompressionWithGUI(CompressionInfos* pCompInfo);

#include "GUI-comp.ipp"


#undef CPM_STARTCOMPRESS
#undef CPM_UPDATEPROGRESS

#endif
