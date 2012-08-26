/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCAppEngine.h
 * Description : 2c start point
 *
 * Created     : 25/04/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCAPPENGINE_H__
#define __CCAPPENGINE_H__


#include "CCEngine.h"

#include "CCScenes.h"
#include "CCObjects.h"


#define CLIENT_NAME "androids"


class CCAppEngine : public CCEngine
{
public:
    typedef CCEngine super;

protected:
    virtual void start();
    
public:
    bool isOrientationSupported(const float angle);
};


#endif // __CCAPPENGINE_H__

