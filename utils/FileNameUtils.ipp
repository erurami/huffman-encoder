
#pragma once

#include <string.h>
#include <stdio.h>



bool IsReadableFile(PSTR lpString)
{
    FILE* pFile;

    fopen_s(&pFile, lpString, "r");

    if (pFile == NULL)
    {
        return false;
    }

    fclose(pFile);

    return true;
}


bool IsWritableFile(PSTR lpString)
{
    FILE* pFile;

    fopen_s(&pFile, lpString, "w");

    if (pFile == NULL)
    {
        return false;
    }

    fclose(pFile);

    return true;
}










int GetFileExtension(
        PSTR lpFileExtension,
        PSTR lpFilePath,
        int  ExtensionBufferSize)
{

    int path_length = lstrlen(lpFilePath);
    int last_period_index = -1;


    for (int i = 0; i < path_length; i++)
    {
        if (lpFilePath[i] == '.')
        {
            last_period_index = i;
        }
    }


    if (last_period_index == -1)
    {
        return GETFE_ERROR_NO_EXTENSION;
    }


    int extension_length = path_length - last_period_index - 1;
    if (extension_length >= ExtensionBufferSize)
    {
        return GETFE_ERROR_TOO_LONG_EXTENSION;
    }



    for (int i = 0; i < extension_length; i++)
    {
        lpFileExtension[i] = lpFilePath[i + last_period_index + 1];
    }
    lpFileExtension[extension_length] = 0;


    return GETFE_SUCCESS;
}






bool IsHuffFile(PSTR lpPath)
{
    TCHAR lp_file_extension[10];

    if (GetFileExtension(lp_file_extension, lpPath, 10) != GETFE_SUCCESS)
    {
        return false;
    }

    if (strcmp(lp_file_extension, TEXT("huff")) != 0)
    {
        return false;
    }

    return true;
}






