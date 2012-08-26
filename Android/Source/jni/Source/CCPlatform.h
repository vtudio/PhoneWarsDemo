/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCPlatform.h
 * Description : Platform specific functions.
 *
 * Created     : 11/05/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCPLATFORM_H__
#define __CCPLATFORM_H__


#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cstdlib>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

#include <pthread.h>

#include <math.h>
#define MAX(x,y)	((x>=y)?x:y)
#define MIN(x,y)	((x>=y)?y:x)

// CCRelease: Remove for release build
//#define PROFILEON

#ifdef DEBUGON
#include <android/log.h>
#define  LOG_TAG    "libccjni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  printf LOGE
#endif

extern void CCNativeThreadLock();
extern void CCNativeThreadUnlock();
extern void CCJobsThreadLock();
extern void CCJobsThreadUnlock();

class CCProfiler
{
public:
	CCProfiler(const char *name);
	~CCProfiler();

	static void open();
	static void save();

protected:
	const char *name;
	double startTime;

	static uint stackIndex;
	static class CCText buffer;
};


#endif // __CCPLATFORM_H__
