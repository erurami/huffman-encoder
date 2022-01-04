
#pragma once


#include <Windows.h>

bool IsReadableFile(PSTR lpString);
bool IsWritableFile(PSTR lpString);


#define GETFE_SUCCESS 0
#define GETFE_ERROR_NO_EXTENSION 1
#define GETFE_ERROR_TOO_LONG_EXTENSION 2

int  GetFileExtension(
        PSTR lpFileExtension,
        PSTR lpFilePath,
        int ExtensionBufferSize);



bool IsHuffFile(PSTR lpPath);





#include "FileNameUtils.ipp"
