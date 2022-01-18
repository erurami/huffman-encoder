
#ifndef _GUI_GUI_HPP_
#define _GUI_GUI_HPP_

#include "windows.h"


// 500 - 599
#define MM_INPUTSELECTED  0x500
#define MM_OUTPUTSELECTED 0x501
#define MM_GENERATEOUTPUT 0x502

#define MAIN_WINDOW_WIDTH  600
#define MAIN_WINDOW_HEIGHT 170


HINSTANCE _G_GUI_GUI_hpp_H_Instance;

int MainApplication(void);

#include "GUI.ipp"


#undef MM_INPUTSELECTED
#undef MM_OUTPUTSELECTED

#endif
