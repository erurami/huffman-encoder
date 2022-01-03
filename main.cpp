
#include <Windows.h>

#include "libs\libs\huffman.hpp"

#include "utils\FileNameUtils.hpp"

#pragma comment(lib,"User32.lib")


#define SHOW_ERROR_AND_RETURN(text) \
    {\
        MessageBox(NULL, text, TEXT("error"), MB_ICONERROR);\
        return -1;\
    }



int WINAPI WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        PSTR lpCmdLine,
        int nCmdShow)
{

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
        // TODO : Open Compression wizard.
        MessageBox(NULL, TEXT("not huff file!"), TEXT(""), MB_ICONINFORMATION);
    }

    return 0;
}
