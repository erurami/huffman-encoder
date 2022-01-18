
#include "GUI.hpp"

#include "resources\Ids.h"

#include <windows.h>
#include <shellapi.h>

#pragma comment(lib, "gdi32.lib"  )
#pragma comment(lib, "Shell32.lib")


#define SHOW_ERROR_AND_RETURN(text) \
    {\
        MessageBox(NULL, text, TEXT("error"), MB_ICONERROR);\
        return -1;\
    }


LRESULT CALLBACK WndprocMainWindow(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp);

int MainApplication(void)
{

    WNDCLASS wincMainWnd;

    wincMainWnd.style         = CS_HREDRAW | CS_VREDRAW;
    wincMainWnd.lpfnWndProc   = WndprocMainWindow;
    wincMainWnd.cbClsExtra    = 0;
    wincMainWnd.cbWndExtra    = 0;
    wincMainWnd.hInstance     = _G_GUI_GUI_hpp_H_Instance;
    wincMainWnd.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wincMainWnd.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wincMainWnd.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wincMainWnd.lpszMenuName  = TEXT("MAINMENU");
    wincMainWnd.lpszClassName = TEXT("MainApplicationWnd");

    if (!RegisterClass(&wincMainWnd))
    {
        SHOW_ERROR_AND_RETURN(TEXT("error registering window class"));
    }


    TCHAR strAppName[100];

    LoadString(_G_GUI_GUI_hpp_H_Instance, STRING_APPLICATION_NAME, strAppName, 100);

    HWND hMainWnd;
    hMainWnd = CreateWindow(
            TEXT("MainApplicationWnd"), strAppName,
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT,
            MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT,
            NULL, NULL, _G_GUI_GUI_hpp_H_Instance, NULL);

    if (hMainWnd == NULL)
    {
        SHOW_ERROR_AND_RETURN(TEXT("error couldn't create window"));
    }



    ShowWindow(hMainWnd, SW_SHOW);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

#undef SHOW_ERROR_AND_RETURN



#define DROPDOWN_ID_EXTORCMP 1
#define BUTTON_ID_BROUSE_OUTPUTFILEPATH 2
#define BUTTON_ID_ENCODE 3

LRESULT CALLBACK WndprocMainWindow(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp)
{
    static LPCTSTR strCompress = TEXT("Compress");
    static LPCTSTR strExtract  = TEXT("Extract");


    static HWND hStaticEncodeType;
    static HWND hComboboxEncodeType;

    static HWND hStaticOutput;
    static HWND hEditOutputFilePath;
    static HWND hButtonBrouseOutputFilePath;

    static HWND hButtonEncode;


    static HFONT hFontText;

    switch (msg)
    {

        case WM_CLOSE:
            if (MessageBox(hWnd, TEXT("quit?"), TEXT("message"), MB_YESNO | MB_ICONQUESTION) == IDYES)
            {
                DeleteObject(hFontText);
                DestroyWindow(hWnd);
                PostQuitMessage(0);
            }
            return 0;



        case WM_CREATE:
            {

            hFontText = CreateFont(
                    15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
                    VARIABLE_PITCH | FF_MODERN, TEXT("Segoe UI"));


            RECT rectClientArea;

            GetClientRect(hWnd, &rectClientArea);

            int window_width  = rectClientArea.right  - rectClientArea.left;
            int window_height = rectClientArea.bottom - rectClientArea.top;


            CreateWindow(
                    TEXT("STATIC"), NULL,
                    WS_CHILD | WS_VISIBLE,
                    0, 0, window_width, window_height,
                    hWnd, NULL, _G_GUI_GUI_hpp_H_Instance, NULL);



            hStaticEncodeType = CreateWindow(
                    TEXT("STATIC"), TEXT("Encode type : "),
                    WS_CHILD | WS_VISIBLE,
                    10, window_height - 65, 80, 20,
                    hWnd, NULL, _G_GUI_GUI_hpp_H_Instance, NULL);

            hComboboxEncodeType = CreateWindow(
                    TEXT("COMBOBOX"), NULL,
                    WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
                    100, window_height - 65, 100, 20,
                    hWnd, (HMENU)DROPDOWN_ID_EXTORCMP, _G_GUI_GUI_hpp_H_Instance, NULL);


            hStaticOutput = CreateWindow(
                    TEXT("STATIC"), TEXT("Output : "),
                    WS_CHILD | WS_VISIBLE,
                    10, window_height - 35, 80, 20,
                    hWnd, NULL, _G_GUI_GUI_hpp_H_Instance, NULL);

            hEditOutputFilePath = CreateWindow(
                    TEXT("EDIT"), TEXT(""),
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                    100, window_height - 35, window_width - 260, 20,
                    hWnd, NULL, _G_GUI_GUI_hpp_H_Instance, NULL);

            hButtonBrouseOutputFilePath = CreateWindow(
                    TEXT("BUTTON"), TEXT("browse"),
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    window_width - 155, window_height - 35, 75, 20,
                    hWnd, (HMENU)BUTTON_ID_BROUSE_OUTPUTFILEPATH, _G_GUI_GUI_hpp_H_Instance, NULL);


            hButtonEncode = CreateWindow(
                    TEXT("BUTTON"), TEXT("Encode"),
                    WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                    window_width - 75, window_height - 65, 70, 50,
                    hWnd, (HMENU)BUTTON_ID_ENCODE, _G_GUI_GUI_hpp_H_Instance, NULL);



            SendMessage(hStaticEncodeType           , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hComboboxEncodeType         , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hStaticOutput               , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hEditOutputFilePath         , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hButtonBrouseOutputFilePath , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hButtonEncode               , WM_SETFONT, (WPARAM)hFontText, TRUE);

            SendMessage(hComboboxEncodeType, CB_ADDSTRING, 0, (LPARAM)strCompress);
            SendMessage(hComboboxEncodeType, CB_ADDSTRING, 0, (LPARAM)strExtract);
            SendMessage(hComboboxEncodeType, CB_SETCURSEL, 0, 0);


            DragAcceptFiles(hWnd, TRUE);

            return 0;
            }



        default:
            return DefWindowProc(hWnd, msg, wp, lp);
    }
}


#undef DROPDOWN_ID_EXTORCMP
#undef BUTTON_ID_BROUSE_OUTPUTFILEPATH
#undef BUTTON_ID_ENCODE















