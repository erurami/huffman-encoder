
#include <Windows.h>

#include "libs\libs\huffman.hpp"

#include "utils\FileNameUtils.hpp"

#pragma comment(lib,"User32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib, "Comdlg32.lib")



#define SHOW_ERROR_AND_RETURN(text) \
    {\
        MessageBox(NULL, text, TEXT("error"), MB_ICONERROR);\
        return -1;\
    }

#define APPLICATION_NAME TEXT("untitled application")



#define CWM_REGISTERFILENAME 0x0400



HINSTANCE G_H_Instance;

int EasyCompressionWizard(PSTR lpFilePath);
int EasyExtractionWizard (PSTR lpFilePath);


int WINAPI WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        PSTR lpCmdLine,
        int nCmdShow)
{

    G_H_Instance = hInstance;



    if (lstrlen(lpCmdLine) == 0)
    {
        // TODO : Open Base Extraction and Compression window.
        MessageBox(NULL, TEXT("no arguments received."), TEXT(""), MB_ICONINFORMATION);
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





#define COMW_WINDOW_HEIGHT 210
#define COMW_WINDOW_WIDTH  500

LRESULT CALLBACK CompressionGuiProc(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp);

LRESULT CALLBACK CompressionWizardProc(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp);

int EasyCompressionWizard(PSTR lpFilePath)
{
    /* ======================== */

    WNDCLASS wincMainWizard;

    wincMainWizard.style         = CS_HREDRAW | CS_VREDRAW;
    wincMainWizard.lpfnWndProc   = CompressionWizardProc;
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
        return -1;
    }




    /* ======================== */

    HWND hWizardWnd;
    hWizardWnd = CreateWindow(
            TEXT("CompressionWizard"), APPLICATION_NAME,
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT,
            COMW_WINDOW_WIDTH, COMW_WINDOW_HEIGHT,
            NULL, NULL, G_H_Instance,
            lpFilePath);

    if (hWizardWnd == NULL)
    {
        SHOW_ERROR_AND_RETURN(TEXT("error couldn't create window"));
        return -1;
    }




    /* ======================== */

    ShowWindow(hWizardWnd, SW_SHOW);


    TCHAR str_full_path[MAX_PATH];

    if (_fullpath(str_full_path, lpFilePath, MAX_PATH) == NULL)
    {
        SHOW_ERROR_AND_RETURN(TEXT("error path is too long"));
    }

    PostMessage(hWizardWnd, CWM_REGISTERFILENAME, (WPARAM)str_full_path, 0);


    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}




HPEN DrawArrowCompression(HDC hdc);

#define SELECT_BUTTON_ID 1
#define KEEPORGFILE_CHECKBOX_ID 2
#define COMPRESS_BUTTON_ID 3
#define CANCEL_BUTTON_ID 4

LRESULT CALLBACK CompressionWizardProc(
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

            hFontText = CreateFont(
                    20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
                    VARIABLE_PITCH | FF_MODERN, TEXT("Segoe UI"));




            hEditFilePathFrom = CreateWindow(
                    TEXT("EDIT"), TEXT(""),
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY,
                    10, 10, COMW_WINDOW_WIDTH - 40, 25,
                    hWnd, NULL, G_H_Instance, NULL);

            hEditFilePathTo = CreateWindow(
                    TEXT("EDIT"), TEXT(""),
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                    10, 70, COMW_WINDOW_WIDTH - 100, 25,
                    hWnd, NULL, G_H_Instance, NULL);



            hSelectFileButton = CreateWindow(
                    TEXT("BUTTON"), TEXT("select"),
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    COMW_WINDOW_WIDTH - 80, 70, 50, 25,
                    hWnd, (HMENU)SELECT_BUTTON_ID, G_H_Instance, NULL);



            CreateWindow(
                    TEXT("STATIC"), TEXT(""),
                    WS_CHILD | WS_VISIBLE,
                    0, 110, COMW_WINDOW_WIDTH, COMW_WINDOW_HEIGHT - 100,
                    hWnd, NULL, G_H_Instance, NULL);



            hKeepOrgFileCheckBox = CreateWindow(
                    TEXT("BUTTON"), TEXT("keep original file"),
                    WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                    10, 130, 200, 25,
                    hWnd, (HMENU)KEEPORGFILE_CHECKBOX_ID, G_H_Instance, NULL);
            SendMessage(hKeepOrgFileCheckBox, BM_SETCHECK, BST_CHECKED, 0);


            hButtonCompress = CreateWindow(
                    TEXT("BUTTON"), TEXT("Compress"),
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    COMW_WINDOW_WIDTH - 240, 130, 100, 25,
                    hWnd, (HMENU)COMPRESS_BUTTON_ID, G_H_Instance, NULL);

            hButtonCancel = CreateWindow(
                    TEXT("BUTTON"), TEXT("Cancel"),
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    COMW_WINDOW_WIDTH - 130, 130, 100, 25,
                    hWnd, (HMENU)CANCEL_BUTTON_ID, G_H_Instance, NULL);



            SendMessage(hEditFilePathFrom   , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hEditFilePathTo     , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hSelectFileButton   , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hKeepOrgFileCheckBox, WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hButtonCompress     , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hButtonCancel       , WM_SETFONT, (WPARAM)hFontText, TRUE);

            return 0;



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

                HPEN hPenUsed = DrawArrowCompression(hdc);

                EndPaint(hWnd, &ps);
                DeleteObject(hPenUsed);
            }
            return 0;



        case WM_COMMAND:
            switch(LOWORD(wp))
            {


                case SELECT_BUTTON_ID:
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


                case COMPRESS_BUTTON_ID:
                    {
                    }
                    break;


                case CANCEL_BUTTON_ID:
                    {
                        PostMessage(hWnd, WM_CLOSE, 0, 0);
                    }


            }
            return 0;



        default:
            return DefWindowProc(hWnd, msg, wp, lp);

    }
}
#undef SELECT_BUTTON_ID
#undef KEEPORGFILE_CHECKBOX_ID
#undef COMPRESS_BUTTON_ID
#undef CANCEL_BUTTON_ID

HPEN DrawArrowCompression(HDC hdc)
{
    HPEN hPen;

    hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));

    MoveToEx(hdc, 20, 40, NULL);
    SelectObject(hdc, hPen);
    LineTo(hdc, 20, 65);
    LineTo(hdc, 30, 55);

    return hPen;
}





















