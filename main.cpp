
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib, "Comdlg32.lib")
#pragma comment(lib, "Shlwapi.lib")

// from microsoft docs
// applies modern designed UIs
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#include "GUI\GUI-comp.hpp"
#include "GUI\GUI-extc.hpp"
#include "GUI\GUI.hpp"

#include "GUI\resources\Ids.h"

#include "utils\FileNameUtils.hpp"


#include <Windows.h>



#define SHOW_ERROR_AND_RETURN(text) \
    {\
        MessageBox(NULL, text, TEXT("error"), MB_ICONERROR);\
        return -1;\
    }



// 400 - 499
#define CWM_REGISTERFILENAME 0x0400



#define COMPWIZD_WINDOW_WIDTH  500
#define COMPWIZD_WINDOW_HEIGHT 210



void SetGlobalHInstance(HINSTANCE hInstance);

HINSTANCE G_H_Instance;

int EasyCompressionWizard(PSTR lpFilePath);
int EasyExtractionWizard (PSTR lpFilePath);


int WINAPI WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        PSTR lpCmdLine,
        int nCmdShow)
{

    SetGlobalHInstance(hInstance);



    if (lstrlen(lpCmdLine) == 0)
    {
        return MainApplication();
    }



    // TODO : this code can't receive multiple files.

    if (IsReadableFile(lpCmdLine) == false)
        SHOW_ERROR_AND_RETURN(TEXT("unreadable file"));



    return MainApplication(lpCmdLine);
}


void SetGlobalHInstance(HINSTANCE hInstance)
{
    G_H_Instance = hInstance;

#ifdef _GUI_GUI_COMP_HPP_
    _G_GUI_GUI_Comp_hpp_H_Instance = hInstance;
#endif

#ifdef _GUI_GUI_EXTC_HPP_
    _G_GUI_GUI_Extc_hpp_H_Instance = hInstance;
#endif

#ifdef _GUI_GUI_HPP_
    _G_GUI_GUI_hpp_H_Instance = hInstance;
#endif
}




