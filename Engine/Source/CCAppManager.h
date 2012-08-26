/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCAppManager.h
 * Description : Interface between the different app specific views and features.
 *
 * Created     : 30/08/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCAPPMANAGER_H__
#define __CCAPPMANAGER_H__

#ifdef IOS
    #ifdef __OBJC__
    #import <QuartzCore/QuartzCore.h>
        #include "CCGLView.h"
        #import "CCViewController.h"
    #else
        #define CCGLView void
        #define CCViewController void
        #define UIWindow void
    #endif
#else
    #include "CCGLView.h"
#endif

class CCAppManager
{
public:
	CCAppManager();
    ~CCAppManager();
    
    void startup();
    void shutdown();
    void pause();
    void resume();
    
    void launchWindow();
    
    inline static bool IsPortrait() { return orientation.target == 0.0f || orientation.target == 180.0f; }
    static void SetOrientation(const float targetOrientation, const bool interpolate=true);
    static void CorrectOrientation(float &x, float &y);
    static void UpdateOrientation(const float delta);
    inline static const CCTarget<float>& GetOrientation() { return orientation; }

protected:
    void toggleBackgroundRender(const bool toggle);
    
protected:
    static CCTarget<float> orientation;
    enum OrientationState
    {
        Orientation_Set,
        Orientation_Updating,
        Orientation_Setting
    };
    static OrientationState orientationState;
    
    // Views
    CCGLView *glView;

#ifdef IOS
public:
	CCViewController *viewController;
protected:
    UIWindow *window;
#endif
    
    bool opaqueOpenGLRendering;
    
public:
    static CCAppManager *appManager;
};


#endif // __CCAPPMANAGER_H__

