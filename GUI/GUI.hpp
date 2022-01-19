
#ifndef _GUI_GUI_HPP_
#define _GUI_GUI_HPP_

#include <windows.h>


// 500 - 599
#define MM_INPUTSELECTED  0x500
#define MM_OUTPUTSELECTED 0x501
#define MM_GENERATEOUTPUT 0x502
#define MM_SETINPUT       0x503
#define MM_STARTENCODE    0x504

#define MAIN_WINDOW_WIDTH  600
#define MAIN_WINDOW_HEIGHT 170


HINSTANCE _G_GUI_GUI_hpp_H_Instance;

int MainApplication(LPTSTR strInputPath = NULL);

#include "GUI.ipp"


#undef MM_INPUTSELECTED
#undef MM_OUTPUTSELECTED
#undef MM_GENERATEOUTPUT
#undef MM_SETINPUT
#undef MM_STARTENCODE

#endif
