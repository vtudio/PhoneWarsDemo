/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCAppManager.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCAppManager.h"

#ifdef QT
#include "CCMainWindow.h"
#elif ANDROID
#include "CCJNI.h"
#endif

#include "CCFileManager.h"


CCGLView *gView = NULL;
CCAppEngine *gEngine = NULL;

CCAppManager *CCAppManager::appManager = NULL;

CCTarget<float> CCAppManager::orientation;
CCAppManager::OrientationState CCAppManager::orientationState = Orientation_Set;


CCAppManager::CCAppManager()
{
    appManager = this;
    
    glView = NULL;

    opaqueOpenGLRendering = true;
}


CCAppManager::~CCAppManager()
{
#ifdef IOS

    [glView release];
    
    [viewController release];
	[window release];

#else

    delete glView;

#endif

    appManager = NULL;
}


void CCAppManager::startup()
{
#ifdef IOS
    // Create a full screen window
    CGRect rect = [[UIScreen mainScreen] bounds];
    
    CGRect statusBarRect = [[UIApplication sharedApplication] statusBarFrame];
    rect.size.height -= statusBarRect.size.height;
    rect.origin.y += statusBarRect.size.height * 0.5f;
    
    window = [[UIWindow alloc] initWithFrame:rect];
    viewController = [[CCViewController alloc] initWithNibName:NULL bundle:NULL];
    
    // Create OpenGL view and add to window
    gEngine = new CCAppEngine();
    glView = [[CCGLView alloc] initWithFrame:rect];
    [glView setViewController:viewController];

#elif defined ANDROID

	// Create our game engine system
	glView = new CCGLView();

#endif

#ifdef IOS
    [window setRootViewController:viewController];
#endif
}


void CCAppManager::shutdown()
{
    gEngine->running = false;

    // Qt isn't multi-threaded yet, on Android this get's called from the rendering thread.
#ifndef IOS
    gEngine->engineThreadRunning = false;
#endif

    while( gEngine->engineThreadRunning )
    {
        usleep( 0 );
    }

    if( gEngine != NULL )
    {
        gEngine->serialize( true );
    }

#ifdef IOS

    [glView shutdown];

#elif defined QT

    glView->shutdown();

#endif
}


void CCAppManager::pause()
{
    if( gEngine != NULL )
    {
        gEngine->pause();
    }
}


void CCAppManager::resume()
{
    if( gEngine != NULL )
    {
        gEngine->resume();
    }
}


void CCAppManager::launchWindow()
{
#ifdef IOS
    
    [CCAppManager::appManager->window makeKeyAndVisible];
    
#endif
}


void CCAppManager::SetOrientation(const float targetOrientation, const bool interpolate)
{
#ifdef IOS
    
    [appManager->viewController setOrientation:targetOrientation];
    
#endif
    
    orientation.target = targetOrientation;
    //if( orientation.current != orientation.target )
    {
        orientationState = interpolate ? Orientation_Updating : Orientation_Setting;
        
        // Update our controls
        CCControls::RefreshTouchMovementThreashold();
    }
}


void CCAppManager::CorrectOrientation(float &x, float &y)
{
    if( orientation.target == 270.0f )
    {
        CCSwapFloat( x, y );
        x = 1.0f - x;
        y = 1.0f - y;
    }
    else if( orientation.target == 90.0f )
    {
        CCSwapFloat( x, y );
    }
    else if( orientation.target == 180.0f )
    {
        x = 1.0f - x;
    }
    else
    {
        y = 1.0f - y;
    }
}


void CCAppManager::UpdateOrientation(const float delta)
{
    if( orientationState != Orientation_Set )
    {
        CCDestructList<CCSceneBase> &scenes = gEngine->scenes;
        for( int i=0; i<scenes.length; ++i )
        {
            CCSceneBase *scene = scenes.list[i];
            scene->beginOrientationUpdate();
        }
        
        // Immediate update?
        if( appManager->orientationState == Orientation_Setting )
        {
            orientation.current = orientation.target - CC_SMALLFLOAT;
        }
        orientationState = Orientation_Set;
    }
    
    if( orientation.current != orientation.target )
    {
        if( CCToRotation( orientation.current, orientation.target, delta * 360.0f ) )
        {
            for( int i=0; i<gEngine->cameras.length; ++i )
            {
                CCCameraBase *camera = gEngine->cameras.list[i];
                camera->flagUpdate();
            }
            
            for( int i=0; i<gEngine->scenes.length; ++i )
            {
                gEngine->scenes.list[i]->finishOrientationUpdate();
            }
        }
    }
}


void CCAppManager::toggleBackgroundRender(const bool toggle)
{
    if( opaqueOpenGLRendering == toggle )
    {
        opaqueOpenGLRendering = !toggle;
#ifdef IOS
        CAEAGLLayer *eaglLayer = (CAEAGLLayer*)glView.layer;
        eaglLayer.opaque = !toggle;
#endif

        // Switch between clearing the alpha channel or not.
        if( opaqueOpenGLRendering )
        {
        	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        }
        else
        {
        	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        }
    }
}
