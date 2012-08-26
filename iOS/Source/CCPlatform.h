/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCPlatform.h
 * Description : Platform specific functions.
 *
 * Created     : 25/01/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCPLATFORM_H__
#define __CCPLATFORM_H__
 

#define IOS

#ifdef __OBJC__

#include <OpenGLES/EAGL.h>

#else

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <cctype>

#include <math.h>
#define MAX(x,y) ((x>=y)?x:y)
#define MIN(x,y) ((x>=y)?y:x)

typedef	unsigned int uint;

#endif

// OpenGL 2.0
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#define glBindVertexArray glBindVertexArrayOES
#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES

//#define _USE_MATH_DEFINES
//#include <math.h>
//
//typedef unsigned int uint;
//
////#include <stdio.h>
//#include <stdlib.h>
//
//#ifndef MAX
//#define MAX(x,y)	((x>=y)?x:y)
//#endif
//
//#ifndef MIN
//#define MIN(x,y)	((x>=y)?y:x)
//#endif

extern void CCNativeThreadLock();
extern void CCNativeThreadUnlock();

extern void CCJobsThreadLock();
extern void CCJobsThreadUnlock();


#endif // __CCPLATFORM_H__