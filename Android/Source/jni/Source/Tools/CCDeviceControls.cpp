/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCDeviceControls.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCDeviceControls.h"
#include "CCAppManager.h"


extern "C" JNIEXPORT void JNICALL Java_com_android2c_CCJNI_ControlsSetDPI(JNIEnv *jEnv, jobject jObj, jfloat jX, jfloat jY)
{
	CCControls::SetDPI( jX, jY );
}


// Java constants pulled from touch event at Java end
enum ActionTypes
{
	ACTION_DOWN  	= 0x0,
	ACTION_UP    	= 0x1,
	ACTION_MOVE  	= 0x2,
	ACTION_CANCEL 	= 0x3,
	ACTION_OUTSIDE 	= 0x3,
	POINTER_DOWN	= 0x5,
	POINTER_UP   	= 0x6
};


extern "C" JNIEXPORT void JNICALL Java_com_android2c_CCJNI_ControlsHandleTouch(JNIEnv *jEnv, jobject jObj, jfloat jX, jfloat jY, jint jActionType, jint jTouchId)
{
	// Force only two fingers
	if( gEngine && ( jTouchId == 0 || jTouchId == 1 ) )
	{
		CCDeviceControls *controls = (CCDeviceControls*)gEngine->controls;
		controls->touchAction( jX, jY, jActionType, jTouchId );
	}
}


extern "C" JNIEXPORT bool JNICALL Java_com_android2c_CCJNI_ControlsHandleBackButton(JNIEnv *jEnv, jobject jObj)
{
	if( gEngine != NULL )
	{
		for( int i=gEngine->scenes.length-1; i>=0; --i )
		{
			CCSceneBase *scene = gEngine->scenes.list[i];
			if( scene->handleBackButton() )
			{
				return true;
			}
		}
	}
	return false;
}


CCDeviceControls::CCDeviceControls()
{
}


void CCDeviceControls::touchAction(const float x, const float y, const int action, const int touchId)
{
	if( action == ACTION_DOWN || action == POINTER_DOWN )
	{
		UITouch *touch = &deviceTouches[touchId];
		touch->position.x = x;
		touch->position.y = y;
		touchBegin( touch, touchId );
	}
	else if( action == ACTION_MOVE )
	{
		UITouch *touch = (UITouch*)screenTouches[touchId].usingTouch;
		if( touch != NULL )
		{
			touch->position.x = x;
			touch->position.y = y;
			touchMove( touch, touchId );
		}
	}
	else
	{
		UITouch *touch = (UITouch*)screenTouches[touchId].usingTouch;
		if( touch != NULL )
		{
			touch->position.x = x;
			touch->position.y = y;
			touchEnd( touch );
		}

	}
}

void CCDeviceControls::touchBegin(UITouch *touch, const int touchIndex)
{
    touchHandle( touch, touchIndex );

    inUse = false;
    for( uint touchIndex=0; touchIndex<numberOfTouches; ++touchIndex )
    {
        inUse |= screenTouches[touchIndex].usingTouch != NULL;
    }
}


void CCDeviceControls::touchMove(UITouch *touch, const int touchIndex)
{
    if( inUse )
    {
        touchHandle( touch, touchIndex );
    }
}


void CCDeviceControls::touchEnd(UITouch *touch)
{
    unTouch( touch );
    bool touchesFinished = true;

    for( uint i = 0; i<numberOfTouches; ++i )
    {
    	if( screenTouches[i].usingTouch != NULL )
    	{
    		touchesFinished = false;
    	}
    }
    
    if( touchesFinished )
    {
        inUse = false;
    }
}


void CCDeviceControls::touchHandle(UITouch *touch, const int touchIndex)
{
    const CCSize &inverseScreenSize = gEngine->renderer->getInverseScreenSize();

    CCPoint position = touch->position;
    position.x *= inverseScreenSize.width;
    position.y *= inverseScreenSize.height;

	CCScreenTouches& screenTouch = screenTouches[touchIndex];
	{
		CCPoint screenPosition = position;
		if( CCAppManager::GetOrientation().target == 270.0f )
		{
			CCSwapFloat( screenPosition.x, screenPosition.y );
			screenPosition.y = 1.0f - screenPosition.y;
		}
		else if( CCAppManager::GetOrientation().target == 90.0f )
		{
			CCSwapFloat( screenPosition.x, screenPosition.y );
			screenPosition.x = 1.0f - screenPosition.x;
		}
		else if( CCAppManager::GetOrientation().target == 180.0f )
		{
			screenPosition.x = 1.0f - screenPosition.x;
			screenPosition.y = 1.0f - screenPosition.y;
		}

		CCNativeThreadLock();
		if( screenTouch.usingTouch != NULL )
		{
			screenTouch.delta.x += screenPosition.x - screenTouch.position.x;
			screenTouch.delta.y += screenPosition.y - screenTouch.position.y;
			screenTouch.totalDelta.x += screenTouch.delta.x;
			screenTouch.totalDelta.y += screenTouch.delta.y;
		}
		else
		{
			// Restart our counters
			screenTouch.totalDelta.x = 0.0f;
			screenTouch.totalDelta.y = 0.0f;
			screenTouch.timeHeld = 0.0f;
			screenTouch.startPosition = screenPosition;

			for( uint i=0; i<CCScreenTouches::max_last_deltas; ++i )
			{
				screenTouch.lastDeltas[i].clear();
			}
		}
		screenTouch.position = screenPosition;
		screenTouch.usingTouch = touch;
		CCNativeThreadUnlock();
	}
}
