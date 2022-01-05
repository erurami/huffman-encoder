
// if you don't want .manifest file in your directory, you can run this command
//      ```
//      mt.exe -manifest main.exe.manifest -outputresource:main.exe;1
//      ```
//
// otherwise you need .manifest to keep visual style of this app, and if you 
// delete .manifest without the command, the app will look "old".
//
// todo : make make
//
// TODO : s/step/stage/g
//           stage means the variable cantains the step or stage of compression / extraction where 
//           the program is running now.
//
// TODO : move all prototype and struct definition to here (beginning of the program)

#include <Windows.h>
#include <Commctrl.h>
#include <shlwapi.h>

#include "libs\libs\huffman.hpp"

#include "utils\FileNameUtils.hpp"

#pragma comment(lib,"User32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib, "Comdlg32.lib")
#pragma comment(lib, "Shlwapi.lib")

// thanks microsoft docs
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")



#define SHOW_ERROR_AND_RETURN(text) \
    {\
        MessageBox(NULL, text, TEXT("error"), MB_ICONERROR);\
        return -1;\
    }

#define APPLICATION_NAME TEXT("untitled application")



#define CWM_REGISTERFILENAME 0x0400
#define CPM_STARTCOMPRESS 0x401
#define CPM_UPDATEPROGRESS 0x402




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




struct CompressionInfos
{
    LPTSTR strFrom;
    LPTSTR strTo;

    bool blKeepOriginal;


    long* npProgress;
    int * npCompressionStage;
};

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

bool StartCompressionWithGUI(CompressionInfos* pCompInfo);

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



#define PROGRESS_WND_WIDTH  400
#define PROGRESS_WND_HEIGHT 220

LRESULT CALLBACK CompressionGuiProc(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp);

bool StartCompressionWithGUI(CompressionInfos* pCompInfo)
{

    WNDCLASS wincProgressGUI;

    wincProgressGUI.style         = CS_HREDRAW | CS_VREDRAW;
    wincProgressGUI.lpfnWndProc   = CompressionGuiProc;
    wincProgressGUI.cbClsExtra    = 0;
    wincProgressGUI.cbWndExtra    = 0;
    wincProgressGUI.hInstance     = G_H_Instance;
    wincProgressGUI.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wincProgressGUI.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wincProgressGUI.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wincProgressGUI.lpszMenuName  = NULL;
    wincProgressGUI.lpszClassName = TEXT("CompressionProgress");


    if (!RegisterClass(&wincProgressGUI))
    {
        MessageBox(NULL, TEXT("error registering window class"), TEXT("error"), MB_ICONERROR);
        return false;
    }





    HWND hProgressWnd;
    hProgressWnd = CreateWindow(
            TEXT("CompressionProgress"), TEXT("Compressing..."),
            WS_TILED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT,
            PROGRESS_WND_WIDTH, PROGRESS_WND_HEIGHT,
            NULL, NULL, G_H_Instance, NULL);

    if (hProgressWnd == NULL)
    {
        MessageBox(NULL, TEXT("error couldn't create window"), TEXT("error"), MB_ICONERROR);
        return false;
    }




    ShowWindow(hProgressWnd, SW_SHOW);

    SendMessage(hProgressWnd, CPM_STARTCOMPRESS, (WPARAM)pCompInfo, 0);

    return true;

}





#define COMPRESSION_END_CODE_SUCCESS         1
#define COMPRESSION_END_CODE_ERROR           2
#define COMPRESSION_END_CODE_FAILED_OPENFILE 3
#define COMPRESSION_END_CODE_USER_CANCEL     4

DWORD WINAPI CompressThreadFunc(LPVOID vdParam);

#define TIMER_ID_UPDATE_PROGRESS 1
#define TIMER_ID_GET_THREAD_STATE 2

#define BUTTON_ID_CANCEL    1
#define BUTTON_ID_PLAYPAUSE 2

LRESULT CALLBACK CompressionGuiProc(
        HWND hWnd,
        UINT msg,
        WPARAM wp,
        LPARAM lp)
{

    static CompressionInfos comp_infos;

    static TCHAR str_compression_description[MAX_PATH * 2 + 50];
    static TCHAR str_progress[100];
    static TCHAR str_compression_stage[100];


    static long n_progress_percentage;
    static int  n_compression_stage;
    static int  n_compression_stage_last_time;

    static bool bl_is_paused;


    static HWND h_progress_bar;

    static HWND h_button_cancel;
    static HWND h_button_playpause;


    static HFONT h_font_description;
    static HFONT h_font_progress;
    static HFONT h_font_compression_stage;


    static HANDLE h_compression_thread;

    static DWORD dw_thread_id;




    switch (msg)
    {



        case WM_CLOSE:
            if (MessageBox(hWnd, TEXT("cancel?"), TEXT("message"), MB_YESNO | MB_ICONINFORMATION) == IDYES)
            {
                // TODO : some canceling process e.g. delete files..
                TerminateThread(h_compression_thread, COMPRESSION_END_CODE_USER_CANCEL);
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

                h_font_compression_stage = CreateFont(
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
                        hWnd, NULL, G_H_Instance, NULL);


                CreateWindow(
                        TEXT("STATIC"), TEXT(""),
                        WS_CHILD | WS_VISIBLE,
                        0, 125, window_width, window_height - 125,
                        hWnd, NULL, G_H_Instance, NULL);

                h_button_cancel = CreateWindow(
                        TEXT("BUTTON"), TEXT("Cancel"),
                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                        window_width - 110, 140, 100, 25,
                        hWnd, (HMENU)BUTTON_ID_CANCEL, G_H_Instance, NULL);

                h_button_playpause = CreateWindow(
                        TEXT("BUTTON"), TEXT("Pause"),
                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                        window_width - 220, 140, 100, 25,
                        hWnd, (HMENU)BUTTON_ID_PLAYPAUSE, G_H_Instance, NULL);


                SendMessage(h_button_cancel   , WM_SETFONT, (WPARAM)h_font_compression_stage, TRUE);
                SendMessage(h_button_playpause, WM_SETFONT, (WPARAM)h_font_compression_stage, TRUE);


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

                    if (n_compression_stage != n_compression_stage_last_time)
                    {
                        switch(n_compression_stage)
                        {

                            case COMPRESSION_STEP_READFREQ:
                                lstrcpy(str_compression_stage, TEXT("Reading file"));
                                break;

                            case COMPRESSION_STEP_BUILDTREE:
                                lstrcpy(str_compression_stage, TEXT("Building tree"));
                                break;

                            case COMPRESSION_STEP_WRITEINFO1:
                                lstrcpy(str_compression_stage, TEXT("Writing header infos"));
                                break;

                            case COMPRESSION_STEP_ENCODE:
                                lstrcpy(str_compression_stage, TEXT("Encoding file"));
                                break;

                            case COMPRESSION_STEP_WRITEINFO2:
                                lstrcpy(str_compression_stage, TEXT("Writing file infos"));
                                break;

                            case COMPRESSION_STEP_FINISHED:
                                lstrcpy(str_compression_stage, TEXT("finished"));
                                break;

                        }
                        InvalidateRect(hWnd, NULL, TRUE);
                        n_compression_stage_last_time = n_compression_stage;
                    }

                    InvalidateRect(hWnd, NULL, FALSE);
                    break;

                case TIMER_ID_GET_THREAD_STATE:
                    {
                        DWORD dw_thread_state;
                        if (!GetExitCodeThread(h_compression_thread, &dw_thread_state))
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

                            case COMPRESSION_END_CODE_SUCCESS:
                                MessageBox(hWnd, TEXT("compression finished"), TEXT(""), MB_ICONINFORMATION);
                                break;

                            case COMPRESSION_END_CODE_ERROR:
                                MessageBox(hWnd, TEXT("an error ocuured"), TEXT(""), MB_ICONINFORMATION);
                                break;

                            case COMPRESSION_END_CODE_FAILED_OPENFILE:
                                MessageBox(hWnd, TEXT("failed to open file"), TEXT(""), MB_ICONINFORMATION);
                                break;

                            case COMPRESSION_END_CODE_USER_CANCEL:
                                break;

                            default:
                                MessageBox(hWnd, TEXT("finifhed with unexpected finish code"), TEXT(""), MB_ICONINFORMATION);
                                break;

                        }

                        DeleteObject(h_font_description);
                        DeleteObject(h_font_progress);
                        DeleteObject(h_font_compression_stage);
                        DestroyWindow(hWnd);
                        PostQuitMessage(0);
                    }
                    break;
            }
            return 0;



        case CPM_STARTCOMPRESS:
            {

                comp_infos = *(CompressionInfos*)wp;

                comp_infos.npProgress         = &n_progress_percentage;
                comp_infos.npCompressionStage = &n_compression_stage;

                h_compression_thread = CreateThread(NULL, 0, CompressThreadFunc, (LPVOID)&comp_infos, 0, &dw_thread_id);


                wsprintf(str_compression_description,
                         TEXT("compressing from %s to %s"),
                         PathFindFileName(comp_infos.strFrom),
                         PathFindFileName(comp_infos.strTo));

                InvalidateRect(hWnd, NULL, FALSE);
            }
            return 0;



        case CPM_UPDATEPROGRESS:
            {
                PostMessage(h_progress_bar, PBM_SETPOS, wp, 0);
            }
            return 0;



        case WM_COMMAND:
            switch (LOWORD(wp))
            {

                case BUTTON_ID_CANCEL:
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    break;

                case BUTTON_ID_PLAYPAUSE:
                    if (bl_is_paused == true)
                    {
                        ResumeThread(h_compression_thread);
                        SetWindowText(h_button_playpause, TEXT("Pause"));
                    }
                    else
                    {
                        SuspendThread(h_compression_thread);
                        SetWindowText(h_button_playpause, TEXT("Play"));
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
                TextOut(hdc, 20, 10, str_compression_description, lstrlen(str_compression_description));

                SelectObject(hdc, h_font_progress);
                TextOut(hdc, 20, 30, str_progress, lstrlen(str_progress));

                SelectObject(hdc, h_font_compression_stage);
                TextOut(hdc, 20, 100, str_compression_stage, lstrlen(str_compression_stage));

                EndPaint(hWnd, &ps);

            }
            return 0;



        default:
            return DefWindowProc(hWnd, msg, wp, lp);
    }
}
#undef TIMER_ID_UPDATE_PROGRESS

#undef BUTTON_ID_CANCEL
#undef BUTTON_ID_PLAYPAUSE

struct ProgressUpdateInfos
{
    int * npStage;
    long* npProgress;
};

void CallbackForCompression(long progress, int stage, void* param);

// TODO : delete original file if the keeporiginal checkbox unchecked.
DWORD WINAPI CompressThreadFunc(LPVOID vdParam)
{
    CompressionInfos comp_infos;
    comp_infos = *(CompressionInfos*)vdParam;


    ProgressUpdateInfos prog_update_infos;
    prog_update_infos.npStage    = comp_infos.npCompressionStage;
    prog_update_infos.npProgress = comp_infos.npProgress;


    // Dummy (for debug)
    // for (int i = 0; i < 101; i++)
    // {
    //     CallbackForCompression(i, i / 20 + 1, (void*)&prog_update_infos);
    //     Sleep(100);
    // }

    FILE* p_file1;
    FILE* p_file2;

    if (fopen_s(&p_file1, comp_infos.strFrom, "rb") != 0)
    {
        ExitThread(COMPRESSION_END_CODE_FAILED_OPENFILE);
    }

    if (fopen_s(&p_file2, comp_infos.strTo, "wb") != 0)
    {
        ExitThread(COMPRESSION_END_CODE_FAILED_OPENFILE);
    }


    Compress(p_file1, p_file2, CallbackForCompression, (void*)&prog_update_infos);


    fclose(p_file1);
    fclose(p_file2);


    ExitThread(COMPRESSION_END_CODE_SUCCESS);
}


void CallbackForCompression(long progress, int stage, void* param)
{
    *(((ProgressUpdateInfos*)param)->npStage)    = stage;

    *(((ProgressUpdateInfos*)param)->npProgress) = progress;
}


















