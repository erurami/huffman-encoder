
#include "GUI-extc.hpp"

#include "libs\libs\huffman.hpp"

#include "wndmessages.h"

#include <windows.h>
#include <Commctrl.h>
#include <shlwapi.h>

#pragma comment(lib,"User32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib, "Comdlg32.lib")
#pragma comment(lib, "Shlwapi.lib")



bool _G_GUI_GUI_extc_ipp_bl_wndclass_registered = false;

LRESULT CALLBACK wndprocExtractionProgress(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp);

bool StartExtractionWithGUI(ExtractionInfos* pExtcInfo, HWND hParentWindow)
{
    if (_G_GUI_GUI_extc_ipp_bl_wndclass_registered)
    {
        goto SKIP_WNDCLASS_REGISTER;
    }

    WNDCLASS wincProgressGUI;

    wincProgressGUI.style         = CS_HREDRAW | CS_VREDRAW;
    wincProgressGUI.lpfnWndProc   = wndprocExtractionProgress;
    wincProgressGUI.cbClsExtra    = 0;
    wincProgressGUI.cbWndExtra    = 0;
    wincProgressGUI.hInstance     = _G_GUI_GUI_Comp_hpp_H_Instance;
    wincProgressGUI.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wincProgressGUI.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wincProgressGUI.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wincProgressGUI.lpszMenuName  = NULL;
    wincProgressGUI.lpszClassName = TEXT("ExtractionProgress");


    if (!RegisterClass(&wincProgressGUI))
    {
        MessageBox(NULL, TEXT("error registering window class (ExtractionProgress)"), TEXT("error"), MB_ICONERROR);
        return false;
    }

    _G_GUI_GUI_extc_ipp_bl_wndclass_registered = true;

SKIP_WNDCLASS_REGISTER:





    HWND hProgressWnd;
    hProgressWnd = CreateWindow(
            TEXT("ExtractionProgress"), TEXT("Extracting..."),
            WS_TILED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT,
            EXTCPROG_WINDOW_WIDTH, EXTCPROG_WINDOW_HEIGHT,
            NULL, NULL, _G_GUI_GUI_Extc_hpp_H_Instance, NULL);

    if (hProgressWnd == NULL)
    {
        MessageBox(NULL, TEXT("error couldn't create window"), TEXT("error"), MB_ICONERROR);
        return false;
    }




    ShowWindow(hProgressWnd, SW_SHOW);

    SendMessage(hProgressWnd, EPM_REGISTERPARENT, (WPARAM)hParentWindow, 0);
    SendMessage(hProgressWnd, EPM_STARTEXTRACT, (WPARAM)pExtcInfo, 0);

    return true;

}




DWORD WINAPI ThreadProcExtract(LPVOID vdParam);


#define TIMER_ID_UPDATE_PROGRESS  1
#define TIMER_ID_GET_THREAD_STATE 2

#define BUTTON_ID_CANCEL    1
#define BUTTON_ID_PLAYPAUSE 2


LRESULT CALLBACK wndprocExtractionProgress(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp)
{

    static ExtractionInfos extc_infos;

    static TCHAR str_extraction_description[MAX_PATH * 2 + 50];
    static TCHAR str_progress[100];
    static TCHAR str_extraction_stage[100];


    static long n_progress_percentage;
    static int  n_extraction_stage;
    static int  n_extraction_stage_last_time;

    static bool bl_is_paused;


    static HWND h_progress_bar;

    static HWND h_button_cancel;
    static HWND h_button_playpause;


    static HFONT h_font_description;
    static HFONT h_font_progress;
    static HFONT h_font_extraction_stage;


    static HANDLE h_extraction_thread;

    static DWORD dw_thread_id;


    static HWND h_parent_window = NULL;




    switch (msg)
    {



        case WM_CLOSE:
            if (MessageBox(hWnd, TEXT("cancel?"), TEXT("message"), MB_YESNO | MB_ICONQUESTION) == IDYES)
            {
                // TODO : some canceling processss
                TerminateThread(h_extraction_thread, _EXTC_ENDCODE_USER_CANCEL);
            }
            return 0;



        case WM_CREATE:
            {

                bl_is_paused = false;


                h_font_description = CreateFont(
                    20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
                    VARIABLE_PITCH | FF_MODERN, TEXT("Segoe UI"));

                h_font_progress = CreateFont(
                    32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
                    VARIABLE_PITCH | FF_MODERN, TEXT("Segoe UI"));

                h_font_extraction_stage = CreateFont(
                    20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
                    VARIABLE_PITCH | FF_MODERN, TEXT("Segoe UI"));



                RECT rectClientArea;

                GetClientRect(hWnd, &rectClientArea);

                int window_width  = rectClientArea.right  - rectClientArea.left;
                int window_height = rectClientArea.bottom - rectClientArea.top;


                h_progress_bar = CreateWindow(
                        PROGRESS_CLASS, TEXT("Progress"),
                        WS_CHILD | WS_VISIBLE,
                        10, 75, window_width - 20, 25,
                        hWnd, NULL, _G_GUI_GUI_Extc_hpp_H_Instance, NULL);


                CreateWindow(
                        TEXT("STATIC"), TEXT(""),
                        WS_CHILD | WS_VISIBLE,
                        0, 125, window_width, window_height - 125,
                        hWnd, NULL, _G_GUI_GUI_Comp_hpp_H_Instance, NULL);

                h_button_cancel = CreateWindow(
                        TEXT("BUTTON"), TEXT("Cancel"),
                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                        window_width - 110, 140, 100, 25,
                        hWnd, (HMENU)BUTTON_ID_CANCEL, _G_GUI_GUI_Comp_hpp_H_Instance, NULL);

                h_button_playpause = CreateWindow(
                        TEXT("BUTTON"), TEXT("Pause"),
                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                        window_width - 220, 140, 100, 25,
                        hWnd, (HMENU)BUTTON_ID_PLAYPAUSE, _G_GUI_GUI_Comp_hpp_H_Instance, NULL);


                SendMessage(h_button_cancel   , WM_SETFONT, (WPARAM)h_font_extraction_stage, TRUE);
                SendMessage(h_button_playpause, WM_SETFONT, (WPARAM)h_font_extraction_stage, TRUE);


                PostMessage(h_progress_bar, PBM_SETRANGE, 0, MAKEWPARAM(0, 100));



                //                                        Å´updating progress frequency
                SetTimer(hWnd, TIMER_ID_UPDATE_PROGRESS , 10, NULL);
                SetTimer(hWnd, TIMER_ID_GET_THREAD_STATE, 100, NULL);
            }
            return 0;



        case WM_TIMER:
            switch (wp)
            {

                case TIMER_ID_UPDATE_PROGRESS:

                    PostMessage(h_progress_bar, PBM_SETPOS, n_progress_percentage, 0);

                    wsprintf(str_progress, TEXT("%d%% complete"), n_progress_percentage);

                    if (n_extraction_stage != n_extraction_stage_last_time)
                    {
                        switch(n_extraction_stage)
                        {

                            case EXTRACTION_STEP_READINFO:
                                lstrcpy(str_extraction_stage, TEXT("Reading informations"));
                                break;

                            case EXTRACTION_STEP_READTREE:
                                lstrcpy(str_extraction_stage, TEXT("Reading informations"));
                                break;

                            case EXTRACTION_STEP_DECODE:
                                lstrcpy(str_extraction_stage, TEXT("Decoding file"));
                                break;

                            case EXTRACTION_STEP_FINISHED:
                                lstrcpy(str_extraction_stage, TEXT("finifhed"));
                                break;

                        }
                        InvalidateRect(hWnd, NULL, TRUE);
                        n_extraction_stage_last_time = n_extraction_stage;
                    }

                    InvalidateRect(hWnd, NULL, FALSE);
                    break;

                case TIMER_ID_GET_THREAD_STATE:
                    {
                        DWORD dw_thread_state;
                        if (!GetExitCodeThread(h_extraction_thread, &dw_thread_state))
                        {
                            return 0;
                        }


                        if (dw_thread_state == STILL_ACTIVE)
                        {
                            return 0;
                        }


                        KillTimer(hWnd, TIMER_ID_GET_THREAD_STATE);

                        switch (dw_thread_state)
                        {

                            case _EXTC_ENDCODE_SUCCESS:
                                MessageBox(hWnd, TEXT("Extraction finished"), TEXT(""), MB_ICONINFORMATION);
                                break;

                            case _EXTC_ENDCODE_ERROR:
                                MessageBox(hWnd, TEXT("an error occured"), TEXT(""), MB_ICONINFORMATION);
                                break;

                            case _EXTC_ENDCODE_FAILED_OPENFILE:
                                MessageBox(hWnd, TEXT("failed to open file"), TEXT(""), MB_ICONINFORMATION);
                                break;

                            case _EXTC_ENDCODE_USER_CANCEL:
                                break;

                            default:
                                MessageBox(hWnd, TEXT("finished with unknown error code"), TEXT(""), MB_ICONINFORMATION);
                                break;

                        }

                        if (h_parent_window != NULL)
                        {
                            SendMessage(h_parent_window, EPM_ENDEXTRACTION, 0, 0);
                        }

                        DeleteObject(h_font_description);
                        DeleteObject(h_font_progress);
                        DeleteObject(h_font_extraction_stage);
                        DestroyWindow(hWnd);
                    }
                    break;
            }
            return 0;



        case EPM_STARTEXTRACT:
            {

                extc_infos = *(ExtractionInfos*)wp;

                extc_infos.npProgress        = &n_progress_percentage;
                extc_infos.npExtractionStage = &n_extraction_stage;

                h_extraction_thread = CreateThread(NULL, 0, ThreadProcExtract, (LPVOID)&extc_infos, 0, &dw_thread_id);


                wsprintf(str_extraction_description,
                        TEXT("extracting from %s to %s"),
                        PathFindFileName(extc_infos.strFrom),
                        PathFindFileName(extc_infos.strTo));

                InvalidateRect(hWnd, NULL, FALSE);
            }
            return 0;



        case EPM_UPDATEPROGRESS:
            {
                PostMessage(h_progress_bar, PBM_SETPOS, wp, 0);
            }
            return 0;



        case EPM_REGISTERPARENT:
            h_parent_window = (HWND)wp;
            return 0;



        case WM_COMMAND:
            switch (LOWORD(wp))
            {

                case BUTTON_ID_CANCEL:
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;

                case BUTTON_ID_PLAYPAUSE:
                    if (bl_is_paused = true)
                    {
                        ResumeThread(h_extraction_thread);
                        SetWindowText(h_button_playpause, TEXT("Pause"));
                    }
                    else
                    {
                        SuspendThread(h_extraction_thread);
                        SetWindowText(h_button_playpause, TEXT("Resume"));
                    }
                    bl_is_paused = !bl_is_paused;
                    break;

            }
            return 0;



        case WM_PAINT:
            {
                HDC hdc;
                PAINTSTRUCT ps;

                hdc = BeginPaint(hWnd, &ps);

                SelectObject(hdc, h_font_description);
                TextOut(hdc, 20, 10, str_extraction_description, lstrlen(str_extraction_description));

                SelectObject(hdc, h_font_progress);
                TextOut(hdc, 20, 30, str_progress, lstrlen(str_progress));

                SelectObject(hdc, h_font_extraction_stage);
                TextOut(hdc, 20, 100, str_extraction_stage, lstrlen(str_extraction_stage));

                EndPaint(hWnd, &ps);

            }
            return 0;



        default:
            return DefWindowProc(hWnd, msg, wp, lp);
    }
}

#undef BUTTON_ID_CANCEL
#undef BUTTON_ID_PLAYPAUSE

#undef TIMER_ID_UPDATE_PROGRESS
#undef TIMER_ID_GET_THREAD_STATE


struct _Extc_ProgressUpdateInfos
{
    int * npStage;
    long* npProgress;
};

void CallbackForExtraction(long progress, int stage, void* param);

// TODO : delete original file if the keeporiginal checkbox unchecked.
DWORD WINAPI ThreadProcExtract(LPVOID vdParam)
{
    ExtractionInfos extc_infos;
    extc_infos = *(ExtractionInfos*)vdParam;


    _Extc_ProgressUpdateInfos prog_update_infos;
    prog_update_infos.npProgress = extc_infos.npProgress;
    prog_update_infos.npStage    = extc_infos.npExtractionStage;


    // Dummy (for debug)
    // 
    // for (int i = 0; i < 101; i++)
    // {
    //     CallbackForExtraction(i, i / 20 + 1, (void*)&prog_update_infos);
    //     Sleep(100);
    // }

    FILE* p_file1;
    FILE* p_file2;

    if (fopen_s(&p_file1, extc_infos.strFrom, "rb") != 0)
    {
        ExitThread(_EXTC_ENDCODE_FAILED_OPENFILE);
    }

    if (fopen_s(&p_file2, extc_infos.strTo  , "wb") != 0)
    {
        ExitThread(_EXTC_ENDCODE_FAILED_OPENFILE);
    }


    Extract(p_file1, p_file2, CallbackForExtraction, (void*)&prog_update_infos);


    fclose(p_file1);
    fclose(p_file2);


    ExitThread(_EXTC_ENDCODE_SUCCESS);
}


void CallbackForExtraction(long progress, int stage, void* param)
{
    *(((_Extc_ProgressUpdateInfos*)param)->npStage)    = stage;

    *(((_Extc_ProgressUpdateInfos*)param)->npProgress) = progress;
}

#undef _EXTC_ENDCODE_SUCCESS
#undef _EXTC_ENDCODE_ERROR
#undef _EXTC_ENDCODE_FAILED_OPENFILE
#undef _EXTC_ENDCODE_USER_CANCEL


