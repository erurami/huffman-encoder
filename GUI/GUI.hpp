
#ifndef _GUI_GUI_HPP_
#define _GUI_GUI_HPP_

#include <windows.h>


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
