
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
        MainApplication();
        return 0;
    }



    // TODO : this code can't receive multiple files.

    if (IsReadableFile(lpCmdLine) == false)
        SHOW_ERROR_AND_RETURN(TEXT("unreadable file"));



    if (IsHuffFile(lpCmdLine))
    {
        // TODO : Open Extraction wizard.
        MessageBox(NULL, TEXT("huff file!"), TEXT(""), MB_ICONINFORMATION);
    }
    else
    {
        return EasyCompressionWizard(lpCmdLine);
    }

    return 0;
}







LRESULT CALLBACK WndprocCompressionWizard(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp);

int EasyCompressionWizard(PSTR lpFilePath)
{

    TCHAR str_full_path[MAX_PATH];

    if (_fullpath(str_full_path, lpFilePath, MAX_PATH) == NULL)
    {
        SHOW_ERROR_AND_RETURN(TEXT("error path is too long"));
    }

    /* ======================== */

    WNDCLASS wincMainWizard;

    wincMainWizard.style         = CS_HREDRAW | CS_VREDRAW;
    wincMainWizard.lpfnWndProc   = WndprocCompressionWizard;
    wincMainWizard.cbClsExtra    = 0;
    wincMainWizard.cbWndExtra    = 0;
    wincMainWizard.hInstance     = G_H_Instance;
    wincMainWizard.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wincMainWizard.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wincMainWizard.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wincMainWizard.lpszMenuName  = NULL;
    wincMainWizard.lpszClassName = TEXT("CompressionWizard");

    if (!RegisterClass(&wincMainWizard))
    {
        SHOW_ERROR_AND_RETURN(TEXT("error registering window class"));
    }




    /* ======================== */

    TCHAR strAppName[100];

    LoadString(G_H_Instance, STRING_APPLICATION_NAME, strAppName, 100);

    HWND hWizardWnd;
    hWizardWnd = CreateWindow(
            TEXT("CompressionWizard"), strAppName,
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT,
            COMPWIZD_WINDOW_WIDTH, COMPWIZD_WINDOW_HEIGHT,
            NULL, NULL, G_H_Instance,
            lpFilePath);

    if (hWizardWnd == NULL)
    {
        SHOW_ERROR_AND_RETURN(TEXT("error couldn't create window"));
    }




    /* ======================== */

    ShowWindow(hWizardWnd, SW_SHOW);


    PostMessage(hWizardWnd, CWM_REGISTERFILENAME, (WPARAM)str_full_path, 0);


    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}




#define BUTTON_ID_SELECT 1
#define BUTTON_ID_KEEPORGFILE 2
#define BUTTON_ID_COMPRESS 3
#define BUTTON_ID_CANCEL 4

LRESULT CALLBACK WndprocCompressionWizard(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp)
{

    static LPTSTR str_file_path_from;
    static TCHAR  str_file_path_to[MAX_PATH];

    static TCHAR str_custom[256] = TEXT("Before files\0*.*\0\0");


    static HWND hEditFilePathFrom;
    static HWND hEditFilePathTo;

    static HWND hSelectFileButton;
    static HWND hKeepOrgFileCheckBox;

    static HWND hButtonCompress;
    static HWND hButtonCancel;


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
                    20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
                    VARIABLE_PITCH | FF_MODERN, TEXT("Segoe UI"));



            RECT rectClientArea;

            GetClientRect(hWnd, &rectClientArea);

            int window_width  = rectClientArea.right  - rectClientArea.left;
            int window_height = rectClientArea.bottom - rectClientArea.top;


            hEditFilePathFrom = CreateWindow(
                    TEXT("EDIT"), TEXT(""),
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY,
                    10, 10, window_width - 20, 25,
                    hWnd, NULL, G_H_Instance, NULL);

            hEditFilePathTo = CreateWindow(
                    TEXT("EDIT"), TEXT(""),
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                    10, 70, window_width - 80, 25,
                    hWnd, NULL, G_H_Instance, NULL);



            hSelectFileButton = CreateWindow(
                    TEXT("BUTTON"), TEXT("select"),
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    window_width - 60, 70, 50, 25,
                    hWnd, (HMENU)BUTTON_ID_SELECT, G_H_Instance, NULL);



            CreateWindow(
                    TEXT("STATIC"), TEXT(""),
                    WS_CHILD | WS_VISIBLE,
                    0, 110, window_width, window_height - 100,
                    hWnd, NULL, G_H_Instance, NULL);



            hKeepOrgFileCheckBox = CreateWindow(
                    TEXT("BUTTON"), TEXT("keep original file"),
                    WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                    10, 130, 200, 25,
                    hWnd, (HMENU)BUTTON_ID_KEEPORGFILE, G_H_Instance, NULL);
            SendMessage(hKeepOrgFileCheckBox, BM_SETCHECK, BST_CHECKED, 0);


            hButtonCompress = CreateWindow(
                    TEXT("BUTTON"), TEXT("Compress"),
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    window_width - 220, 130, 100, 25,
                    hWnd, (HMENU)BUTTON_ID_COMPRESS, G_H_Instance, NULL);

            hButtonCancel = CreateWindow(
                    TEXT("BUTTON"), TEXT("Cancel"),
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    window_width - 110, 130, 100, 25,
                    hWnd, (HMENU)BUTTON_ID_CANCEL, G_H_Instance, NULL);



            SendMessage(hEditFilePathFrom   , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hEditFilePathTo     , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hSelectFileButton   , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hKeepOrgFileCheckBox, WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hButtonCompress     , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hButtonCancel       , WM_SETFONT, (WPARAM)hFontText, TRUE);

            return 0;
            }



        case CWM_REGISTERFILENAME:

            str_file_path_from = (LPTSTR)wp;
            lstrcpyn(str_file_path_to, str_file_path_from, 1000);
            lstrcat (str_file_path_to, TEXT(".huff"));

            SetWindowText(hEditFilePathFrom, str_file_path_from);
            SetWindowText(hEditFilePathTo  , str_file_path_to);

            InvalidateRect(hWnd, NULL, TRUE);
            return 0;



        case WM_PAINT:
            {

                HDC hdc;
                PAINTSTRUCT ps;

                RECT rectClient;

                hdc = BeginPaint(hWnd, &ps);
                GetClientRect(hWnd, &rectClient);
                SelectObject(hdc, hFontText);

                HPEN hPenUsed = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));

                MoveToEx(hdc, 20, 40, NULL);
                SelectObject(hdc, hPenUsed);
                LineTo(hdc, 20, 65);
                LineTo(hdc, 30, 55);

                EndPaint(hWnd, &ps);
                DeleteObject(hPenUsed);
            }
            return 0;



        case WM_COMMAND:
            switch(LOWORD(wp))
            {


                case BUTTON_ID_SELECT:
                    {
                        OPENFILENAME ofn = {0};

                        ofn.lStructSize = sizeof (OPENFILENAME);
                        ofn.hwndOwner = hWnd;

                        ofn.lpstrFilter = TEXT("Huff files {*.huff}\0*.huff\0")
                                          TEXT("All files {*.*}\0*.*\0");
                        ofn.lpstrCustomFilter = str_custom;
                        ofn.nMaxCustFilter = 256;
                        ofn.nFilterIndex = 0;

                        ofn.lpstrFile = str_file_path_to;
                        ofn.nMaxFile = MAX_PATH;

                        ofn.Flags = OFN_EXPLORER;

                        ofn.lpstrDefExt = TEXT("huff");


                        GetSaveFileName(&ofn);
                        SetWindowText(hEditFilePathTo, str_file_path_to);
                    }
                    break;


                case BUTTON_ID_COMPRESS:
                    {
                        CompressionInfos comp_infos;

                        comp_infos.strFrom        = str_file_path_from;
                        comp_infos.strTo          = str_file_path_to;

                        if (SendMessage(hKeepOrgFileCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED)
                        {
                            comp_infos.blKeepOriginal = true;
                        }
                        else
                        {
                            comp_infos.blKeepOriginal = false;
                        }


                        if (StartCompressionWithGUI(&comp_infos) == true)
                        {
                            DeleteObject(hFontText);
                            DestroyWindow(hWnd);
                        }
                    }
                    break;


                case BUTTON_ID_CANCEL:
                    {
                        PostMessage(hWnd, WM_CLOSE, 0, 0);
                    }


            }
            return 0;



        default:
            return DefWindowProc(hWnd, msg, wp, lp);

    }
}
#undef BUTTON_ID_SELECT
#undef BUTTON_ID_KEEPORGFILE
#undef BUTTON_ID_COMPRESS
#undef BUTTON_ID_CANCEL


void SetGlobalHInstance(HINSTANCE hInstance)
{
    G_H_Instance = hInstance;

#ifdef _GUI_GUI_COMP_HPP_
    _G_GUI_GUI_Comp_hpp_H_Instance = hInstance;
#endif

#ifdef _GUI_GUI_EXTC_HPP_
    _G_GUI_GUI_Extc_hpp_H_Instance = hInstance;
#endif
}




