/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCSceneAppUI.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"


CCSceneAppUI::CCSceneAppUI()
{
    camera = NULL;
    cameraCentered = false;
    cameraScrolling = false;
    // s = ( ( u+v ) / 2 ) * t;
    // u+v = ( s / t ) * 2
    //float distance = ( velocity * time ) + ( ( -velocity * pow( time, 2 ) ) / 2 );
    //    if( cameraVelocity != 0.0f )
    //    {
    //        cameraLookAt.y += cameraVelocity * time.delta;
    //        ToTarget( &cameraVelocity, 0.0f, cameraReleaseVelocity * time.delta * 2.0f );
    //        camera->flagUpdate();
    //        
    //        if( cameraScrolling && fabsf( cameraVelocity ) < 10.0f )
    //        {
    //            cameraScrolling = false;
    //            lockCameraView();
    //        }
    //    }

    controlsUITouch = NULL;
    controlsMoving = false;

    updatingOrientation = false;
}


void CCSceneAppUI::setup()
{
    if( camera == NULL )
    {
        camera = new CCCameraAppUI();
        gEngine->addCamera( camera );
        camera->setupViewport( 0.0f, 0.0f, 1.0f, 1.0f );
    }
    camera->setCameraWidth( 100.0f );
    
    refreshCameraView();
    lockCameraView();
}


void CCSceneAppUI::destruct()
{
    orientationCallbacks.deleteObjectsAndList();

    if( camera != NULL )
    {
        gEngine->removeCamera( camera );
        delete camera;
    }
    
    super::destruct();
}


// CCSceneBase
bool CCSceneAppUI::updateControls(const CCTime &time)
{	
    if( camera->getFrameBufferID() != -1 )
    {
        return false;
    }
    
    if( updatingOrientation )
    {
        return false;
    }

    const CCScreenTouches *touches = camera->getRelativeTouches();
    bool usingControls = handleTouches( touches[0], touches[1], time );
    if( usingControls == false )
    {
        return super::updateControls( time );
    }
    
    return usingControls;
}


bool CCSceneAppUI::handleTouches(const CCScreenTouches &touch1, const CCScreenTouches &touch2, const CCTime &time)
{
    bool usingControls = false;
    
    // Handles two touches pressed
	if( touch1.usingTouch != NULL && touch2.usingTouch != NULL )
	{
        if( touchAllowed( touch1 ) )
        {
            if( controlsUITouch == touch1.usingTouch )
            {
                touchReleased( touch1, touch_lost );
            }
            
            if( controlsUITouch != touch2.usingTouch )
            {
                twoTouchRegistered( touch1, touch2 );
            }
            
            usingControls = handleTwoTouches( touch1, touch2 );
        }
	}
	else
	{
		// Handles one touch pressed
		if( touch2.lastTimeReleased > 0.0f &&
            touchAllowed( touch1 ) )
		{
			if( controlsUITouch != touch1.usingTouch )
			{
                touchRegistered( touch1 );
			}
			
            usingControls = handleOneTouch( touch1 );
		}
		
		// On touch release
		else 
        {
            if( controlsUITouch != NULL )
            {
                if( controlsUITouch == touch1.lastTouch && touch1.usingTouch != NULL )
                {	
                    usingControls = touchReleased( touch1, touch_released );
                }
                else
                {
                    touchReleased( touch1, touch_lost );
                }
                
                lockCameraView();
                
                // Reset state
                if( controlsMoving )
                {
                    controlsMoving = false;
                }
				controlsUITouch = NULL;
			}
		}
	}
    
    return usingControls;
}


bool CCSceneAppUI::updateScene(const CCTime &time)
{
    return super::updateScene( time );
}


bool CCSceneAppUI::updateCamera(const CCTime &time)
{
    bool updated = false;
    
    const float lookAtSpeed = controlsUITouch && cameraScrolling == false ? 20.0f : 1.5f;
	if( camera->interpolateCamera( time.delta, lookAtSpeed ) )
    {
        updated = true;
	}
    else
    {
        if( cameraScrolling )
        {
            cameraScrolling = false;
            lockCameraView();
            updated = true;
        }
        
        if( updatingOrientation )
        {
            updatingOrientation = false;
            refreshCameraView();
            lockCameraView();
            updated = true;
        }
    }
    
    return updated;
}


bool CCSceneAppUI::render(const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    if( camera == inCamera )
    {
        renderObjects( inCamera, pass, alpha );
        return true;
    }
    return false;
}


void CCSceneAppUI::renderOctreeObject(CCObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    if( camera == inCamera )
	{
        object->renderObject( inCamera, alpha );
	}
}


void CCSceneAppUI::render2DBackground(const uint inCameraIndex)
{
    if( inCameraIndex == camera->getIndex() )
    {
        for( int i=0; i<childScenes.length; ++i )
        {
            childScenes.list[i]->render2DBackground( inCameraIndex );
        }
    }
}


void CCSceneAppUI::render2DForeground(const uint inCameraIndex)
{
    if( inCameraIndex == camera->getIndex() )
    {
        for( int i=0; i<childScenes.length; ++i )
        {
            childScenes.list[i]->render2DForeground( inCameraIndex );
        }
    }
}


void CCSceneAppUI::beginOrientationUpdate()
{
    if( camera->getFrameBufferID() != -1 )
    {
        return;
    }
    
    camera->recalcViewport();
    
    const float previousCameraHeight = camera->targetHeight;
    camera->setCameraWidth( camera->targetWidth );
    if( cameraCentered == false )
    {
        const float newCameraHeight = camera->targetHeight;
        const float heightDifference = previousCameraHeight - newCameraHeight;
        camera->targetLookAt.y += heightDifference * 0.5f;
    }

    for( int i=0; i<orientationCallbacks.length; ++i )
    {
        orientationCallbacks.list[i]->run();
    }
    
    updatingOrientation = true;
}


void CCSceneAppUI::finishOrientationUpdate()
{
    if( camera->getFrameBufferID() != -1 )
    {
        return;
    }
    
    updatingOrientation = true;
}


bool CCSceneAppUI::touchAllowed(const CCScreenTouches &touch)
{
    if( enabled )
    {
        // If the touch started in this scene
        if( touch.usingTouch != NULL && touch.lastTouch == NULL )
        {
            return touch.startPosition.x >= 0.0f && touch.startPosition.x <= 1.0f &&
                   touch.startPosition.y >= 0.0f && touch.startPosition.y <= 1.0f;   
        }
        return false;

        // If the touch started and is in this scene
        return touch.usingTouch != NULL && 
        touch.position.x >= 0.0f && touch.position.x <= 1.0f &&
        touch.position.y >= 0.0f && touch.position.y <= 1.0f &&
        touch.startPosition.x >= 0.0f && touch.startPosition.x <= 1.0f &&
        touch.startPosition.y >= 0.0f && touch.startPosition.y <= 1.0f;

        // Potentially required for frame buffer controls?
        return touch.usingTouch != NULL && 
        touch.position.x > camera->getViewportX() && touch.position.x < camera->getViewportX2() &&
        touch.position.y > camera->getViewportY() && touch.position.y < camera->getViewportY2() &&
        touch.startPosition.x > camera->getViewportX() && touch.startPosition.x < camera->getViewportX2() &&
        touch.startPosition.y > camera->getViewportY() && touch.startPosition.y < camera->getViewportY2();
    }
    return false;
}


bool CCSceneAppUI::handleOneTouch(const CCScreenTouches &touch1)
{
    bool usingControls = false;
    
    CCPoint touchDelta = touch1.delta;
    if( controlsMoving == false )
    {
        if( touchMovementAllowed( touch1, touchDelta ) )
        {
        }
        else
        {
            usingControls = touchPressed( touch1 );
        }
    }
    
    if( controlsMoving )
    {   
        usingControls = touchMoving( touch1, touchDelta );
    }
    
    return usingControls;
}


bool CCSceneAppUI::handleTwoTouches(const CCScreenTouches &touch1, const CCScreenTouches &touch2)
{
//    CCPoint movement;
//    // If x's are going in different directions?
//#ifdef TARGET_IPHONE_SIMULATOR
//    movement.x = -touch1.delta.x;
//    movement.y = touch1.delta.y;
//    const CCPoint movementAbs = CCPoint( fabsf( movement.x ), fabsf( movement.y ) );
//    if( movementAbs.y > movementAbs.x )
//#else
//        movement.x = -touch1.delta.x + -touch2.delta.x;
//    movement.y = touch1.delta.y + touch2.delta.y;
//    if( CCControls::detectZoomTouch( touch1, touch2 ) )
//#endif
//    {
//        // Find out the position of our touches
//        const CCScreenTouches *topTouch = &touch1,
//        *bottomTouch = &touch2;
//        if( touch1.position.y < touch2.position.y )
//        {
//            topTouch = &touch2;
//            bottomTouch = &touch1;
//        }
//        const CCScreenTouches *rightTouch = &touch1,
//        *leftTouch = &touch2;
//        if( touch1.position.x < touch2.position.x )
//        {
//            rightTouch = &touch2;
//            leftTouch = &touch1;
//        }
//        const float combinedDelta = topTouch->delta.y + rightTouch->delta.x + -bottomTouch->delta.y + -leftTouch->delta.x;
//        return touchCameraZooming( combinedDelta );
//    }
//    else
    {
        return touchCameraRotating( touch1.delta.x, touch1.delta.y );
    }
}


bool CCSceneAppUI::handleThreeTouches(const CCScreenTouches &touch1,
                                            const CCScreenTouches &touch2, 
                                            const CCScreenTouches &touch3)
{
    return false;
}


void CCSceneAppUI::touchRegistered(const CCScreenTouches &touch)
{
    controlsUITouch = touch.usingTouch;
}


void CCSceneAppUI::twoTouchRegistered(const CCScreenTouches &touch1, const CCScreenTouches &touch2)
{
    controlsUITouch = touch2.usingTouch;
}


bool CCSceneAppUI::touchPressed(const CCScreenTouches &touch)
{
    return handleTilesTouch( touch, touch_pressed ) == tile_touchAction;
}


bool CCSceneAppUI::touchMovementAllowed(const CCScreenTouches &touch, CCPoint &touchDelta)
{
    const float absDeltaX = fabsf( touch.totalDelta.x );
    const float absDeltaY = fabsf( touch.totalDelta.y );
    const CCPoint& touchMovementThreashold = CCControls::GetTouchMovementThreashold();
    if( absDeltaX > touchMovementThreashold.x || absDeltaY > touchMovementThreashold.y )
    {
        controlsMoving = true;
        touchDelta.x += touch.totalDelta.x;
        touchDelta.y += touch.totalDelta.y;
        
        controlsMovingVertical = absDeltaY > absDeltaX;
        return true;
    }
    return false;
}


bool CCSceneAppUI::touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta)
{
    // Run through all the tiles
    CCTileTouchResult result = handleTilesTouch( touch, controlsMovingVertical ? touch_movingVertical : touch_movingHorizontal );
    if( result == tile_touchAction )
    {
        return true;
    }
    else
    {
        return touchCameraMoving( touch, touchDelta.x, touchDelta.y );
    }
}


bool CCSceneAppUI::touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction)
{
    // Find pressed tile
    CCTileTouchResult result = handleTilesTouch( touch, touchAction );
    bool usingControls = result == tile_touchAction;
    if( usingControls == false && touchAction == touch_released )
    {
        usingControls = touchReleaseSwipe( touch );
    }
    return usingControls;
}


CCSceneAppUI::CCTileTouchResult CCSceneAppUI::handleTilesTouch(const CCScreenTouches &touch, const CCTouchAction touchAction)
{
    if( camera->project3D( touch.position.x, touch.position.y ) )
    {
        const CCCameraProjectionResults &projectionResults = camera->getProjectionResults();
        
        // Scan to see if we're blocked by a collision
        static CCVector3 hitPosition;
        CCObjectCollideable *hitObject = CCBasicLineCollisionCheck( (CCObjectCollideable**)tiles.list,
                                                                   tiles.length,
                                                                   NULL,
                                                                   projectionResults.vNear, projectionResults.vFar,
                                                                   &hitPosition,
                                                                   true,
                                                                   collision_ui,
                                                                   false );
        
        // Fill in the hitPosition variable if nothing has been hit
        if( hitObject == NULL )
        {
            hitPosition = projectionResults.vLookAt;
        }

        bool actioned = false;
        for( int i=0; i<tiles.length; ++i )
        {
            CCTile3D *tile = tiles.list[i];
            if( tile->handleProjectedTouch( projectionResults, hitObject, hitPosition, touch, touchAction ) )
            {
                actioned |= true;
                if( touchAction < touch_released )
                {
                    break;
                }
            }
        }
        
        if( actioned )
        {
            return tile_touchAction;
        }
        
        if( hitObject != NULL )
        {
            return tile_touchCollision;
        }
    }
    return tile_touchNone;
}


bool CCSceneAppUI::touchCameraMoving(const CCScreenTouches &touch, const float x, const float y)
{
    if( controlsMovingVertical )
    {
        if( y != 0.0f )
        {
            float delta = y * camera->cameraHeight;
            if( camera->targetLookAt.y > sceneTop || camera->targetLookAt.y < sceneBottom )
            {
                delta *= 0.5f;
            }
            camera->targetLookAt.y += delta;
            camera->flagUpdate();
            return true;
        }
    }
    else
    {
        if( x != 0.0f )
        {
            float delta = x * camera->cameraWidth;
            if( camera->targetLookAt.x < sceneLeft || camera->targetLookAt.x > sceneRight )
            {
                delta *= 0.5f;
            }
            camera->targetLookAt.x -= delta;
            camera->flagUpdate();
            return true;
        }
    }
    return true;
}


bool CCSceneAppUI::touchCameraZooming(const float amount)
{
    camera->targetOffset.z -= amount * 2.0f * camera->getOffset().z;
    CCClampFloat( camera->targetOffset.z, 1.0f, camera->targetOffset.z );
    camera->flagUpdate();
    return true;
}


bool CCSceneAppUI::touchCameraRotating(const float x, const float y)
{
    CCVector3 rotation = camera->getRotationVector();
    
    rotation.y += -x * 180.0f;
    CCClampFloat( rotation.y, -45.0f, 45.0f );
    camera->setRotationY( rotation.y );
    
    rotation.x += -y * 180.0f;
    CCClampFloat( rotation.x, -45.0f, 45.0f );
    camera->setRotationX( rotation.x );
    
    camera->flagUpdate();
    return true;
}


bool CCSceneAppUI::touchReleaseSwipe(const CCScreenTouches &touch)
{
    const float maxTimeHeld = 0.5f;
    if( touch.timeHeld < maxTimeHeld )
    {
        const float minMovementThreashold = 0.1f;
        if( controlsMovingVertical )
        {
            if( touch.totalDelta.y < -minMovementThreashold || 
                touch.totalDelta.y > minMovementThreashold )
            {
                cameraScrolling = true;
                const CCPoint averageLastDelta = touch.averageLastDeltas();
                const float displacement = averageLastDelta.y * camera->cameraHeight * 5.0f;
                float cameraLookAtTarget = camera->targetLookAt.y + displacement;
                if( cameraLookAtTarget > sceneTop )
                {
                    const float distance = cameraLookAtTarget - sceneTop;
                    cameraLookAtTarget -= distance * 0.25f;
                    cameraLookAtTarget = MIN( cameraLookAtTarget, sceneTop + camera->cameraHHeight * 0.5f );
                }
                else if( cameraLookAtTarget < sceneBottom )
                {
                    const float distance = cameraLookAtTarget - sceneBottom;
                    cameraLookAtTarget -= distance * 0.25f;
                    cameraLookAtTarget = MAX( cameraLookAtTarget, sceneBottom - camera->cameraHHeight * 0.5f );
                }
                camera->targetLookAt.y = cameraLookAtTarget;
                camera->flagUpdate();
                return true;
            }
        }
        else
        {
            if( touch.totalDelta.x < -minMovementThreashold )
            {
                camera->targetLookAt.x += camera->cameraHWidth;
                camera->flagUpdate();
                return true;
            }
            else if( touch.totalDelta.x > minMovementThreashold )
            {
                camera->targetLookAt.x -= camera->cameraHWidth;
                camera->flagUpdate();
                return true;
            }
        }
    }
    return false;
}


void CCSceneAppUI::refreshCameraView()
{
    sceneLeft = 0.0f;
    sceneRight = 0.0f;
    sceneTop = 0.0f;
    sceneBottom = 0.0f;
}


// Lock the camera on something interesting
void CCSceneAppUI::lockCameraView()
{
    if( cameraScrolling )
    {
        return;
    }
    
    camera->flagUpdate();
    camera->targetLookAt.x = 0.0f;
    camera->targetLookAt.y = 0.0f;
}


void CCSceneAppUI::scrollCameraToTop()
{
    camera->targetLookAt.y = sceneTop;
    camera->flagUpdate();
}
