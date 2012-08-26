/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCFileManager.h
 * Description : Handles loading files between platforms.
 *
 * Created     : 11/05/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCFILEMANAGER_H_
#define __CCFILEMANAGER_H_


#include "CCBaseTypes.h"
#include "CCCallbacks.h"
#include <sys/stat.h>

class CCIOCallback : public CCLambdaCallback
{
public:
    CCIOCallback(const int inPriority)
    {
        priority = inPriority;
        exists = false;
    }
    
    // Run this before IO is performed to verify the lambda of the call is still active
    virtual bool isCallbackActive() = 0;
    
    CCText filePath;
    int priority;
    bool exists;
};


class CCFileManager
{
    // data pointer must be freed with a call to FREE_POINTER
    static int GetFileData(const char *fullFilePath, char **data, CCResourceType resourceType, const bool assertOnFail=true);
    
public:
    static void GetFilePath(CCText &fullFilePath, const char *filePath, CCResourceType resourceType);

    static int GetPackagedFile(const char *filePath, char **data, const bool assertOnFail=true, struct stat *info=NULL);

    static int GetCachedFile(const char *filePath, char **data, const bool assertOnFail=true, struct stat *info=NULL);
    static bool SaveCachedFile(const char *filePath, const char *data, const int length);
    static bool DeleteCachedFile(const char *filePath);

    static void ReadyIO();
    static void DoesCachedFileExistAsync(const char *filePath, CCIOCallback *inCallback);
    static bool DoesFileExist(const char *filePath, CCResourceType resourceType=Resource_Cached);
};


#endif // __CCFILEMANAGER_H_
