/*----------------------------------------------------------
* Copyright © 2011 - 2011 France Telecom
* This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
*-----------------------------------------------------------
* File Name   : main.cpp
* Description : JNI entry and interface point.
*
* Created     : 15/05/11
* Author(s)   : Ashraf Samy Hegab, Chris Bowers
*-----------------------------------------------------------
*/

#include "CCDefines.h"
#include "CCAppEngine.h"
#include "CCGLView.h"
#include "CCDeviceControls.h"

#include "CCAppManager.h"

#include <jni.h>


static void printGLString(const char *name, GLenum s)
{
    const char *v = (const char *)glGetString( s );
    DEBUGLOG( "GL %s = %s\n", name, v );
}


extern "C" JNIEXPORT void JNICALL Java_com_android2c_CCJNI_OnSurfaceChanged(JNIEnv *jEnv, jobject jObj, jint jWidth, jint jHeight)
{	
#ifdef DEBUGON
	// Now we can do what we intended
	printGLString( "Version", GL_VERSION );
	printGLString( "Vendor", GL_VENDOR );
	printGLString( "Renderer", GL_RENDERER );
	printGLString( "Extensions", GL_EXTENSIONS );
#endif

	if( CCAppManager::appManager == NULL )
	{
		new CCAppManager();

		// Get the window controller to create the glView
		CCAppManager::appManager->startup();
	}

	// Then feed it in its appropriate settings here
	gView->jniEnv = jEnv;
	gView->resizeView( jWidth, jHeight );

	if( gEngine != NULL )
	{
		delete gEngine;
		gEngine = NULL;
	}

	gEngine = new CCAppEngine();
	gEngine->setupNativeThread();
	gEngine->setupEngineThread();

#if defined PROFILEON
    CCProfiler::open();
#endif
}


extern "C" JNIEXPORT void JNICALL Java_com_android2c_CCJNI_OnDrawFrame(JNIEnv *jEnv, jobject jObj)
{
	if( gView != NULL )
	{
		gView->jniEnv = jEnv;	// This doesn't change, but just in case.

		if( gEngine->running )
		{
			gEngine->updateNativeThread();
			gEngine->updateJobsThread();
			gEngine->updateEngineThread();
		}
	}

#if defined PROFILEON
    CCProfiler::save();
#endif
}


extern "C" JNIEXPORT void JNICALL Java_com_android2c_CCJNI_AppResumed(JNIEnv *jEnv, jobject jObj)
{
	if( gEngine != NULL )
	{
		class ThreadCallback : public CCLambdaCallback
		{
		public:
			void run()
			{
				if( gEngine != NULL )
				{
					gEngine->resume();
				}
			}
		};
	    gEngine->nativeToEngineThread( new ThreadCallback() );
	}
}
