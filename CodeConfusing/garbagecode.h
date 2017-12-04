//
//  garbagecode.h
//  CodeConfusing
//
//  Created by Apple on 2017/12/4.
//
//

#ifndef garbagecode_H
#define garbagecode_H

#include "srcfilemodel.h"

class GarbageCodeTool
{
public:
    int insertGarbageCode(SrcFileModel srcFile, string garbageCode);
    void modifyContentInFile(const char *fileName,string content);
};

#endif /* garbagecode_H */
