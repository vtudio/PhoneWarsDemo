/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCCameraAppUI.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCAppManager.h"


CCCameraAppUI::CCCameraAppUI()
{
}


void CCCameraAppUI::update()
{
    super::update();
}


void CCCameraAppUI::setLookAt(const CCVector3 &newLookAt)
{
    super::setLookAt( newLookAt );
    targetLookAt = currentLookAtTarget.current = currentLookAtTarget.target = lookAt;
}


void CCCameraAppUI::setOffset(const CCVector3 &newOffset)
{
    updateOffset( newOffset );
    targetOffset = currentOffsetTarget.current = currentOffsetTarget.target = offset;
}


bool CCCameraAppUI::interpolateCamera(const float delta, const float speed)
{
	if( updating )
	{   
        updating = false;
        
        if( currentOffsetTarget.target != targetOffset )
        {
            currentOffsetTarget.target = targetOffset;
            offsetInterpolator.setup( &currentOffsetTarget.current, currentOffsetTarget.target );
        }
        
        if( currentLookAtTarget.target != targetLookAt )
        {
            currentLookAtTarget.target = targetLookAt;
            lookAtInterpolator.setup( &currentLookAtTarget.current, currentLookAtTarget.target );
        }
        
        if( offsetInterpolator.update( delta * speed ) )
        {
            refreshCameraSize();
            updating = true;
        }
        
        if( lookAtInterpolator.update( delta * speed ) )
        {
            updating = true;
            updatedPosition = true;
        }
        
        // Now update our camera
        {
            lookAt = currentLookAtTarget.current;
            updateOffset( currentOffsetTarget.current );
        }
        return true;
	}

	return false;
}


void CCCameraAppUI::setCameraWidth(const float inWidth, const bool interpolate)
{
    targetWidth = cameraWidth = inWidth;
    targetOffset.z = cameraWidth;
    if( frameBufferID != -1 || CCAppManager::IsPortrait() )
    {
        targetOffset.z /= getFrustumSize().width;
        cameraHeight = targetOffset.z * getFrustumSize().height;
    }
    else
    {
        targetOffset.z /= getFrustumSize().height;
        cameraHeight = targetOffset.z * getFrustumSize().width;
    }
    
    targetHeight = cameraHeight;
    cameraHWidth = cameraWidth * 0.5f;
    cameraHHeight = cameraHeight * 0.5f;
    
    flagUpdate();
    if( interpolate == false )
    {
        setOffset( targetOffset );
    }
}


void CCCameraAppUI::setCameraHeight(const float inHeight, const bool interpolate)
{
    targetHeight = cameraHeight = inHeight;
    targetOffset.z = cameraHeight;
    if( frameBufferID != -1 || CCAppManager::IsPortrait() )
    {
        targetOffset.z /= getFrustumSize().height;
        cameraWidth = targetOffset.z * getFrustumSize().width;
    }
    else
    {
        targetOffset.z /= getFrustumSize().width;
        cameraWidth = targetOffset.z * getFrustumSize().height;
    }
    
    targetWidth = cameraWidth;
    cameraHWidth = cameraWidth * 0.5f;
    cameraHHeight = cameraHeight * 0.5f;
    
    flagUpdate();
    if( interpolate == false )
    {
        setOffset( targetOffset );
    }
}


void CCCameraAppUI::refreshCameraSize()
{
    CCVector3 &currentOffset = currentOffsetTarget.current;
    if( frameBufferID != -1 || CCAppManager::IsPortrait() )
    {
        cameraWidth = currentOffset.z * getFrustumSize().width;
        cameraHeight = currentOffset.z * getFrustumSize().height;
    }
    else
    {
        cameraWidth = currentOffset.z * getFrustumSize().height;
        cameraHeight = currentOffset.z * getFrustumSize().width;
    }
    
    cameraHWidth = cameraWidth * 0.5f;
    cameraHHeight = cameraHeight * 0.5f;
}



float CCCameraAppUI::calcCameraOffset(const float inWidth)
{
    float offsetZ = inWidth;
    if( frameBufferID != -1 || CCAppManager::IsPortrait() )
    {
        offsetZ /= getFrustumSize().width;
    }
    else
    {
        offsetZ /= getFrustumSize().height;
    }
    
    return offsetZ;
}


float CCCameraAppUI::calcCameraWidth(const float inOffset)
{
    float width = inOffset;
    if( frameBufferID != -1 || CCAppManager::IsPortrait() )
    {
        width *= getFrustumSize().width;
    }
    else
    {
        width *= getFrustumSize().height;
    }
    
    return width;
}


float CCCameraAppUI::calcCameraHeight(const float inOffset)
{
    float height = inOffset;
    if( frameBufferID != -1 || CCAppManager::IsPortrait() )
    {
        height *= getFrustumSize().height;
    }
    else
    {
        height *= getFrustumSize().width;
    }
    
    return height;
}