
#include "GUI.hpp"

#include "resources\Ids.h"

#include "..\utils\FileNameUtils.hpp"

#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>

#pragma comment(lib, "gdi32.lib"  )
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")


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
#define BUTTON_ID_KEEPORIGINAL 4

LRESULT CALLBACK WndprocMainWindow(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp)
{
    static LPCTSTR strCompress = TEXT("Compress");
    static LPCTSTR strExtract  = TEXT("Extract");

    static TCHAR str_custom[256] = TEXT("Before files\0*.*\0\0");


    static BOOL  bl_is_input_selected  = false;
    static BOOL  bl_is_output_selected = false;

    static TCHAR str_file_path_input[MAX_PATH];
    static TCHAR str_file_path_output[MAX_PATH];


    static HWND hStaticBackground;

    static HWND hEditInputFilePath;

    static HWND hStaticEncodeType;
    static HWND hComboboxEncodeType;
    static HWND hCheckBoxKeepOriginal;

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


            hStaticBackground = CreateWindow(
                    TEXT("STATIC"), NULL,
                    WS_CHILD | WS_VISIBLE,
                    0, 0, window_width, window_height,
                    hWnd, NULL, _G_GUI_GUI_hpp_H_Instance, NULL);



            hEditInputFilePath = CreateWindow(
                    TEXT("EDIT"), TEXT(""),
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
                    10, 10, window_width - 20, 20,
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

            hCheckBoxKeepOriginal = CreateWindow(
                    TEXT("BUTTON"), TEXT("Keep original file"),
                    WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                    210, window_height - 63, 150, 20,
                    hWnd, (HMENU)BUTTON_ID_KEEPORIGINAL, _G_GUI_GUI_hpp_H_Instance, NULL);


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



            SendMessage(hEditInputFilePath          , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hStaticEncodeType           , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hComboboxEncodeType         , WM_SETFONT, (WPARAM)hFontText, TRUE);
            SendMessage(hCheckBoxKeepOriginal       , WM_SETFONT, (WPARAM)hFontText, TRUE);
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



        case WM_PAINT:
            {
                HDC hdc;
                PAINTSTRUCT ps;

                hdc = BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);

                // hdc = BeginPaint(hStaticBackground, &ps);
                // SelectObject(hdc, hFontText);

                // if (bl_is_input_selected)
                // {
                //     TextOut(hdc, 10, 10, str_file_path_input, lstrlen(str_file_path_input));
                // }

                // if (bl_is_output_selected)
                // {
                //     TextOut(hdc, 10, 70, str_file_path_output, lstrlen(str_file_path_output));
                // }

                // EndPaint(hStaticBackground, &ps);
            }
            return 0;



        case WM_COMMAND:
            switch (LOWORD(wp))
            {

                case DROPDOWN_ID_EXTORCMP:
                    switch (HIWORD(wp))
                    {

                        case CBN_SELCHANGE:
                            SendMessage(hWnd, MM_GENERATEOUTPUT, 0, 0);
                            break;

                    }
                    break;


                case BUTTON_ID_BROUSE_OUTPUTFILEPATH:
                    if (bl_is_input_selected == false)
                    {
                        MessageBox(hWnd, TEXT("Please Select input file first"), TEXT(""), MB_ICONINFORMATION);
                    }
                    else
                    {
                        OPENFILENAME ofn = {0};

                        ofn.lStructSize = sizeof (OPENFILENAME);
                        ofn.hwndOwner = hWnd;

                        ofn.lpstrFilter = TEXT("All files {*.*}\0*.*\0");
                        ofn.lpstrCustomFilter = str_custom;
                        ofn.nMaxCustFilter = 256;
                        ofn.nFilterIndex = 0;

                        ofn.lpstrFile = str_file_path_output;
                        ofn.nMaxFile = MAX_PATH;

                        ofn.Flags = OFN_EXPLORER;

                        ofn.lpstrDefExt = NULL;


                        GetSaveFileName(&ofn);

                        SendMessage(hWnd, MM_OUTPUTSELECTED, 0, 0);
                    }
                    break;


                case BUTTON_ID_ENCODE:
                    {
                        int n_encode_type = SendMessage(hComboboxEncodeType, CB_GETCURSEL, 0, 0);
                        if (n_encode_type == 0)
                        {
                            CompressionInfos comp_infos;

                            comp_infos.strFrom        = str_file_path_input;
                            comp_infos.strTo          = str_file_path_output;

                            if (SendMessage(hCheckBoxKeepOriginal, BM_GETCHECK, 0, 0) == BST_CHECKED)
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
                        else if (n_encode_type == 1)
                        {
                            ExtractionInfos extc_infos;

                            extc_infos.strFrom        = str_file_path_input;
                            extc_infos.strTo          = str_file_path_output;

                            if (SendMessage(hCheckBoxKeepOriginal, BM_GETCHECK, 0, 0) == BST_CHECKED)
                            {
                                extc_infos.blKeepOriginal = true;
                            }
                            else
                            {
                                extc_infos.blKeepOriginal = false;
                            }


                            // TODO : HIDE window while extraction, SHOW window after extraction.
                            if (StartExtractionWithGUI(&extc_infos) == true)
                            {
                                DeleteObject(hFontText);
                                DestroyWindow(hWnd);
                            }
                        }
                        break;
                    }


                case MENUID_OPEN:
                    {
                        OPENFILENAME ofn = {0};

                        ofn.lStructSize = sizeof (OPENFILENAME);
                        ofn.hwndOwner = hWnd;

                        ofn.lpstrFilter = TEXT("All files {*.*}\0*.*\0");
                        ofn.lpstrCustomFilter = str_custom;
                        ofn.nMaxCustFilter = 256;
                        ofn.nFilterIndex = 0;

                        ofn.lpstrFile = str_file_path_input;
                        ofn.nMaxFile = MAX_PATH;

                        ofn.Flags = OFN_EXPLORER;

                        ofn.lpstrDefExt = NULL;


                        GetOpenFileName(&ofn);

                        SendMessage(hWnd, MM_INPUTSELECTED, 0, 0);
                    }
                    break;


                case MENUID_EXIT:
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;

            }
            return 0;



        case WM_MENUSELECT:
            return 0;



        case WM_DROPFILES:
            {
                HDROP hDroppedInfos;

                hDroppedInfos = (HDROP)wp;

                // todo : Support multiple files.
                // TODO : do not accept foders.
                DragQueryFileA(hDroppedInfos, 0, str_file_path_input, MAX_PATH);

                DragFinish(hDroppedInfos);

                SendMessage(hWnd, MM_INPUTSELECTED, 0, 0);
            }
            return 0;



        case MM_GENERATEOUTPUT:
            {
            if (lstrlen(str_file_path_input) == 0)
            {
                break;
            }

            int n_encode_type = SendMessage(hComboboxEncodeType, CB_GETCURSEL, 0, 0);
            if (n_encode_type == 1)
            {
                lstrcpyn(str_file_path_output, str_file_path_input, MAX_PATH - 5);
                *PathFindExtension(str_file_path_output) = 0;

                if (IsReadableFile(str_file_path_output))
                {
                    // copy extension
                    TCHAR str_extension[MAX_PATH];
                    lstrcpyn(str_extension, PathFindExtension(str_file_path_output), MAX_PATH);

                    // remove extension
                    *PathFindExtension(str_file_path_output) = 0;

                    // add "(0)"
                    lstrcat(str_file_path_output, TEXT("(0)"));

                    // add extension
                    lstrcat(str_file_path_output, str_extension);
                }
            }
            else if (n_encode_type == 0)
            {
                lstrcpyn(str_file_path_output, str_file_path_input, MAX_PATH - 5);
                lstrcat (str_file_path_output, TEXT(".huff"));
            }

            SendMessage(hWnd, MM_OUTPUTSELECTED, 0, 0);

            return 0;
            }



        case MM_INPUTSELECTED:

            if (IsHuffFile(str_file_path_input))
            {
                SendMessage(hComboboxEncodeType, CB_SETCURSEL, 1, 0);
            }
            else
            {
                SendMessage(hComboboxEncodeType, CB_SETCURSEL, 0, 0);
            }

            SendMessage(hWnd, MM_GENERATEOUTPUT, 0, 0);

            SetWindowText(hEditInputFilePath, str_file_path_input);
            bl_is_input_selected = true;

            InvalidateRect(hWnd, NULL, TRUE);
            return 0;



        case MM_OUTPUTSELECTED:

            SetWindowText(hEditOutputFilePath, str_file_path_output);
            bl_is_output_selected = true;

            InvalidateRect(hWnd, NULL, TRUE);
            return 0;



        default:
            return DefWindowProc(hWnd, msg, wp, lp);
    }
}


#undef DROPDOWN_ID_EXTORCMP
#undef BUTTON_ID_BROUSE_OUTPUTFILEPATH
#undef BUTTON_ID_ENCODE
#undef BUTTON_ID_KEEPORIGINAL















