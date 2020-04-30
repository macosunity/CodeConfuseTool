#pragma once

#include "srcfilemodel.h"

class GarbageCodeTool
{
public:
    void insertGarbageCode(SrcFileModel srcFile, string garbageCode);
    void insertIncludeCode(SrcFileModel srcFile, string includeCode);
    void modifyContentInFile(const char *fileName,string content);
};
