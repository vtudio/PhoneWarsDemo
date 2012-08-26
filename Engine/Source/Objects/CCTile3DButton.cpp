/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCTile3DButton.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCObjects.h"
#include "CCTextureFontPage.h"
#include "CCControllerMovement.h"


CCTile3DButton::CCTile3DButton(CCSceneBase *scene)
{
    construct( scene );
}


void CCTile3DButton::setupTile(const float size, const char *text)
{
    setupTile( size, size, text );
}


void CCTile3DButton::setupTile(const float width, const float height, const char *text)
{
    setTileSize( width, height );
    
    if( text )
    {
        textModel->setText( text, height );
    }
}


void CCTile3DButton::setupTextured(const char *textureName, const CCResourceType resourceType, const bool mipmap)
{
    setTileSize();
    setTileTexture( textureName, Resource_Packaged, mipmap );
    setTileTexturedSize();
}


void CCTile3DButton::setupTexturedWidth(const float width, const char *textureName, const CCResourceType resourceType, const bool mipmap)
{
    setTileSize();
    setTileTexture( textureName, Resource_Packaged, mipmap );
    setTileTexturedWidth( width );
}


void CCTile3DButton::setupTexturedHeight(const float height, const char *textureName, const CCResourceType resourceType, const bool mipmap)
{
    setTileSize();
    setTileTexture( textureName, Resource_Packaged, mipmap );
    setTileTexturedHeight( height );
}


void CCTile3DButton::setupTexturedFit(const float width, const float height, const char *textureName, const bool mipmap)
{
    setTileSize();
    setTileTexture( textureName, Resource_Packaged, mipmap );
    setTileTexturedFit( width, height );
}


void CCTile3DButton::setupText(const char *text, const float textHeight, const bool centered, const bool background)
{
    textModel->setText( text, textHeight );
    
    const float width = textModel->getWidth();
    const float height = textModel->getHeight();
    
    setCollisionBounds( width, height, CC_SMALLFLOAT );
    
    textModel->setCentered( centered );
    
    if( centered == false )
    {
        translate( collisionBounds.x, 0.0f, 0.0f );
    }
    
    if( background )
    {
        setTileSize( width, height );
    }
}


void CCTile3DButton::construct(CCSceneBase *scene)
{
    CCAddFlag( collideableType, collision_ui );

    if( scene != NULL )
    {
        setScene( scene );
    }
    
    setTransparent();
    
    model = new CCModelBase();
    
    tileModel = new CCModelBase();
    model->addModel( tileModel );
    setTileColour( CCColour(), false );
    tileColourInterpolator.setDuration( 0.5f );
    tileSquare = NULL;
    
    // Use an object to ensure the model is rendered in the transparent pass
    textObject = new CCObject();
    textModel = new CCModelText( this );
    textObject->model = textModel;
    addChild( textObject );
    textObject->setTransparent( true );
    
    allowTouchRotation( false );
    touchRotationMagnitude = 0.0f;
    touchRotationSpeed = 1.0f;
    
    setTileScale( CCVector3( 1.0f ), true );

    allowTouchMovement( false );

    touchDepressInterpolator.setDuration( 0.125f );
    setTouchDepressRange( 1.0f );
    setTouchDepressDepth( 3.0f );

    CCUpdateCollisions( this );
    
    moving = false;
}


void CCTile3DButton::destruct()
{
    super::destruct();
}

// CCRenderable
void CCTile3DButton::refreshModelMatrix()
{
	if( updateModelMatrix )
	{
        CCMatrixLoadIdentity( modelMatrix );	
        CCMatrixTranslate( modelMatrix, position.x, position.y, position.z );
        CCMatrixTranslate( modelMatrix, touchDepressPosition.x, touchDepressPosition.y, touchDepressPosition.z );
        
		if( scale != NULL )
		{	
			CCMatrixScale( modelMatrix, scale->x, scale->y, scale->z );
		}

        if( rotation.x != 0.0f )
        {
            CCMatrixRotate( modelMatrix, rotation.x, 1.0f, 0.0f, 0.0f );
        }

        if( rotation.y != 0.0f )
        {
            CCMatrixRotate( modelMatrix, rotation.y, 0.0f, 1.0f, 0.0f );
        }
        
        if( rotation.z != 0.0f )
        {
            CCMatrixRotate( modelMatrix, rotation.z, 0.0f, 0.0f, 1.0f );
        }
        
		if( touchRotationMagnitude != 0.0f )
        {
            CCMatrixRotate( modelMatrix, touchRotationMagnitude * 20.0f, 1.0f, 0.0f, 0.0f );
        }
        
		updateModelMatrix = false;
		dirtyWorldMatrix();
	}
}


void CCTile3DButton::setPositionXYZ(const float x, const float y, const float z)
{
    super::setPositionXYZ( x, y, z );
    if( moving )
    {
        moving = false;
        touchMovementInterpolator.clear();
    }
}


// CCObject
bool CCTile3DButton::update(const CCTime &time)
{
    bool updated = super::update( time );

    if( scale != NULL )
    {
        if( tileScaleInterpolator.update( time.delta ) )
        {
            dirtyModelMatrix();
            updated = true;
        }
    }

    // Touch depress: On update
    if( touchDepressRange > 0.0f )
    {
        if( touchDepressInterpolator.update( time.delta ) )
        {
            dirtyModelMatrix();
            updated = true;
        }
    }

    if( touchRotationAllowed )
    {
        const float speed = touching || touchReleased ? 3.0f : touchRotationSpeed;
        if( touchRotationInterpolator.update( time.delta * speed ) )
        {
            const float magnitudeSquared = rotation.x * rotation.x + rotation.y * rotation.y;
            touchRotationMagnitude = sqrtf( magnitudeSquared );
            touchRotationMagnitude = MIN( touchRotationMagnitude, 1.0f );

            dirtyModelMatrix();
            updated = true;
        }
    }

    if( touching )
    {
        touchingTime += time.real;
    }
    else if( touchReleased )
    {
        if( touchDepressInterpolator.finished() && touchRotationInterpolator.finished() )
        {
            handleTouchRelease();
            updated = true;
        }
    }

    updated |= tileColourInterpolator.update( time.delta );

    if( textModel != NULL )
    {
        updated |= textModel->update( time.delta );
    }
    
    
    if( moving )
    {
        if( touchMovementInterpolator.update( time.delta ) )
        {
            updateCollisions = true;
            CCOctreeRefreshObject( this );
            dirtyModelMatrix();
            updated = true;
        }
        else
        {
            moving = false;
        }
    }
    
    return updated;
}


void CCTile3DButton::renderModel(const bool alpha)
{
    super::renderModel( alpha );
}


void CCTile3DButton::setTileSize(const float size)
{
    setTileSize( size, size );
}


void CCTile3DButton::setTileSize(const float width, const float height)
{
    if( tileSquare == NULL )
    {
        tileSquare = new CCPrimitiveSquare();
        tileModel->addPrimitive( tileSquare );
    }

    setCollisionBounds( width, height, CC_SMALLFLOAT );
    CCUpdateCollisions( this );
    
    tileSquare->setupZFacing( collisionBoundsLength.width, collisionBoundsLength.height );
}


void CCTile3DButton::setTileTexture(const char *name, const CCResourceType resourceType, const bool mipmap, const bool load)
{
    tileSquare->setTexture( name, resourceType, mipmap, load );
}


void CCTile3DButton::setTileTexturedSize()
{
    ASSERT( tileSquare != NULL && tileSquare->textureInfo != NULL );
    
    const CCTextureBase *texture = gEngine->textureManager->getTexture( tileSquare->textureInfo->primaryIndex );
    const float width = texture->getRawWidth();
    const float height = texture->getRawHeight();
    setTileSize( width, height );
}


void CCTile3DButton::setTileTexturedWidth(const float width)
{
    ASSERT( tileSquare != NULL && tileSquare->textureInfo != NULL );
        
    const CCTextureBase *texture = gEngine->textureManager->getTexture( tileSquare->textureInfo->primaryIndex );
    ASSERT( texture != NULL );
    const float aspectRatio = texture->getRawWidth() / texture->getRawHeight();
    const float height = width / aspectRatio;
    setTileSize( width, height );
}


void CCTile3DButton::setTileTexturedHeight(const float height)
{
    ASSERT( tileSquare != NULL && tileSquare->textureInfo != NULL );
    
    const CCTextureBase *texture = gEngine->textureManager->getTexture( tileSquare->textureInfo->primaryIndex );
    ASSERT( texture != NULL );
    const float aspectRatio = texture->getRawWidth() / texture->getRawHeight();
    const float width = height * aspectRatio;
    setTileSize( width, height );
}


void CCTile3DButton::setTileTexturedFit(const float width, const float height)
{
    ASSERT( tileSquare != NULL && tileSquare->textureInfo != NULL );
    
    const CCTextureBase *texture = gEngine->textureManager->getTexture( tileSquare->textureInfo->primaryIndex );
    const float targetAspectRatio = width / height;
    const float textureAspectRatio = texture->getRawWidth() / texture->getRawHeight();
    
    if( textureAspectRatio > targetAspectRatio )
    {
        setTileTexturedWidth( width );
    }
    else
    {
        setTileTexturedHeight( height );
    }
}


void CCTile3DButton::setTileColour(const CCColour &inColour, const bool interpolate)
{
    if( tileModel == NULL )
    {
        setTileSize( 0.0f, 0.0f );
    }
    
    if( interpolate )
    {
        tileColourInterpolator.setup( tileModel->getColour(), inColour );
    }
    else
    {
        tileModel->setColour( inColour );
        tileColourInterpolator.setup( tileModel->getColour(), *tileModel->getColour() );
    }
}


void CCTile3DButton::setTileColourAlpha(const float inAlpha, const bool interpolate, CCLambdaCallback *inCallback)
{
    if( interpolate )
    {
        tileColourInterpolator.setTargetAlpha( inAlpha, inCallback );
    }
    else 
    {
        tileModel->setColourAlpha( inAlpha );
        tileColourInterpolator.setup( tileModel->getColour(), *tileModel->getColour() );
    }
}


void CCTile3DButton::setTileScale(const CCVector3 inScale, const bool interpolate, CCLambdaCallback *onInterpolated)
{
    if( scale == NULL )
    {
        CCVector3FillPtr( &scale, 0.0f, 0.0f, 1.0f );
    }
    
    if( interpolate )
    {
        tileScaleInterpolator.setup( scale, inScale, onInterpolated );
    }
    else 
    {
        *scale = inScale;
        tileScaleInterpolator.setup( scale, *scale );
    }
}



void CCTile3DButton::setTileMovement(const CCVector3 target)
{
    moving = true;
    touchMovementInterpolator.pushV3( positionPtr, target, true );
}


void CCTile3DButton::setTileMovementX(const float x)
{
    setTileMovement( CCVector3( x, position.y, position.z ) );
}


void CCTile3DButton::translateTileMovementX(const float x)
{
    setTileMovementX( position.x + x );
}


void CCTile3DButton::setTileMovementY(const float y)
{
    setTileMovement( CCVector3( position.x, y, position.z ) );
}


void CCTile3DButton::setTileMovementBelow(const CCTile3DButton *fromTile)
{
    CCVector3 target = fromTile->getTileMovementTarget();
    target.y -= ( fromTile->collisionBounds.y + collisionBounds.y );
    setTileMovement( target );
}


const CCVector3 CCTile3DButton::getTileMovementTarget() const
{
    if( moving )
    {
        return touchMovementInterpolator.getTarget();
    }
    return position;
}


void CCTile3DButton::resetTileUVs()
{
    if( tileSquare != NULL )
    {
        if( tileSquare->customUVs != NULL )
        {
            DELETE_POINTER( tileSquare->customUVs );
        }
    }
}


void CCTile3DButton::flipTileY()
{
    if( tileSquare != NULL )
    {
        if( tileSquare->customUVs != NULL )
        {
            tileSquare->customUVs->flipY();
        }
        else
        {
            CCPrimitiveSquareUVs::Setup( &tileSquare->customUVs, 1.0f, 0.0f, 0.0f, 1.0f );
        }
    }
}


void CCTile3DButton::setTextPosition(const float x, const float y)
{
    textObject->setPositionX( x * collisionBounds.x );
    textObject->setPositionY( y * collisionBounds.y );
}


void CCTile3DButton::setTextFlash(const bool toggle)
{
    if( toggle )
    {
        LAMBDA_1( AnimationCallback, CCTile3DButton, tile,
                 {
                     const float currentAlpha = tile->getTextModel()->getColourInterpolator().getTarget().alpha;
                     tile->setTextColourAlpha( currentAlpha == 1.0f ? 0.5f : 1.0f, true, new AnimationCallback( tile ) );
                 });
        setTextColourAlpha( 1.0f, true, new AnimationCallback( this ) );
    }
    else
    {
        setTextColourAlpha( 1.0f, true, NULL );
    }
}


bool CCTile3DButton::handleProjectedTouch(const CCCameraProjectionResults &cameraProjectionResults,
                                          const CCObjectCollideable *hitObject,
                                          const CCVector3 &hitPosition,
                                          const CCScreenTouches &touch,
                                          const CCTouchAction touchAction)
{
    if( enabled == false )
    {
        return false;
    }

    if( hitObject == this && 
        ( touchAction == touch_pressed || ( touchMovementAllowed && CCControls::TouchActionMoving( touchAction ) ) ) )
    {
        if( touching == false )
        {
            touching = true;
            touchingTime = 0.0f;
            onTouchPress();
        }
    }
    
    if( touching )
    {
        const float maxTimeHeld = 0.125f;
        
        if( touchAction == touch_pressed )
        {
            if( touch.timeHeld >= maxTimeHeld )
            {
                CCVector3 relativeHitPosition;
                relativeHitPosition.x = hitPosition.x - position.x;
                relativeHitPosition.y = hitPosition.y - position.y;
                float x = relativeHitPosition.x / collisionBounds.x;
                float y = relativeHitPosition.y / collisionBounds.y;
                touchActionPressed( x, y, touch, touchAction );
            }
        }
        else if( touchMovementAllowed && CCControls::TouchActionMoving( touchAction ) )
        {
            if( hitObject != this )
            {
                return handleProjectedTouch( cameraProjectionResults, hitObject, hitPosition, touch, touch_lost );
            }
            else
            {
                CCVector3 relativeHitPosition;
                relativeHitPosition.x = hitPosition.x - position.x;
                relativeHitPosition.y = hitPosition.y - position.y;
                float x = relativeHitPosition.x / collisionBounds.x;
                float y = relativeHitPosition.y / collisionBounds.y;
                touchActionPressed( x, y, touch, touchAction );
            }
        }
        else
        {   
            // Ensure we have a good touch release
            if( touchAction == touch_released )
            {   
                if( touchMovementAllowed == false )
                {
                    const float absDeltaX = fabsf( touch.totalDelta.x );
                    const float absDeltaY = fabsf( touch.totalDelta.y );
                    if( absDeltaX > CCControls::GetTouchMovementThreashold().x || absDeltaY > CCControls::GetTouchMovementThreashold().y )
                    {
                        return handleProjectedTouch( cameraProjectionResults, hitObject, hitPosition, touch, touch_lost );
                    }
                }
                
                // If we have a good first touch, register it.
                CCVector3 relativeHitPosition;
                relativeHitPosition.x = hitPosition.x - position.x;
                relativeHitPosition.y = hitPosition.y - position.y;
                float x = relativeHitPosition.x / collisionBounds.x;
                float y = relativeHitPosition.y / collisionBounds.y;
                touchActionPressed( x, y, touch, touch_pressed );
            }

            touching = false;
            touchActionRelease( touchAction );
            
            if( touchAction == touch_released )
            {
                return true;
            }
        }
    }
    
    return false;
}


void CCTile3DButton::touchActionPressed(const float x, const float y, const CCScreenTouches &touch, const CCTouchAction touchAction)
{
    if( touchRotationAllowed )
    {
        float clampedX = x;
        float clampedY = y;
        CCClampFloat( clampedX, -1.0f, 1.0f );
        CCClampFloat( clampedY, -1.0f, 1.0f );
        touchRotationInterpolator.pushV3( &rotation, CCVector3( -clampedY, clampedX, 0.0f ), true );
    }
    
    // Touch depress: On pressed
    if( touchDepressRange > 0.0f )
    {
        // Touch depress range specifies the area which the button is pressed down
        // If it's out of range the button is pushed back up
        if( fabsf( x ) < touchDepressRange && fabsf( y ) < touchDepressRange )
        {
            touchDepressInterpolator.pushV3( &touchDepressPosition, CCVector3( 0.0f, 0.0f, -touchDepressDepth ), true );
        }
        else
        {
            touchDepressInterpolator.pushV3( &touchDepressPosition, CCVector3(), true );
        }
    }

    if( touchAction > touch_pressed && touchAction < touch_released )
    {
        touchActionMoved( x,  y,  touch, touchAction);
    }
}


void CCTile3DButton::touchActionMoved(const float x, const float y, const CCScreenTouches &touch, const CCTouchAction touchAction)
{
    onTouchMove();
}


void CCTile3DButton::touchActionRelease(const CCTouchAction touchAction)
{
    if( touchAction == touch_released )
    {
        touchReleased = true;
    }
    else 
    {
        handleTouchRelease();
        if( touchAction == touch_lost )
        {
            onTouchLoss();
        }
    }
}


void CCTile3DButton::handleTouchRelease()
{
    // If the touch has been released successfully we fire our callback
    if( touchReleased )
    {
        touchReleased = false;
        onTouchRelease();
    }

    if( touchRotationAllowed )
    {
        touchRotationInterpolator.pushV3( &rotation, CCVector3() );
    }

    // Touch depress: On releasse
    if( touchDepressRange > 0.0f )
    {
        touchDepressInterpolator.pushV3( &touchDepressPosition, CCVector3(), true );
    }
}
