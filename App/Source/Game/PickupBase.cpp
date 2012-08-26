/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CollideableFloor.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"
#include "CCModelObj.h"


PickupBase::PickupBase(const char *name, const char *type, const char *pickupID)
{
    model3d = NULL;
	CCAddFlag( collideableType, collision_static );
    
    this->name = name;
    this->type = type;
    this->pickupID = pickupID;
}


// ObjectBase
bool PickupBase::update(const CCTime &time)
{
    if( model != NULL )
    {
        model->rotateY( time.delta * -90.0f );
    }
    return false;
}


void PickupBase::renderModel(const bool alpha)
{
    if( alpha )
    {
        GLEnableDepth();
    }
    
    glDisable( GL_CULL_FACE );
	super::renderModel( alpha );
    glEnable( GL_CULL_FACE );
    
    if( alpha )
    {
        GLDisableDepth();
    }
}


bool PickupBase::shouldCollide(CCObjectCollideable *collideWith, const bool initialCall)
{
	return super::shouldCollide( collideWith, initialCall );
}


CCObjectCollideable* PickupBase::recieveCollisionFrom(CCObjectCollideable *collisionSource, const float x, const float y, const float z)
{	
	return super::recieveCollisionFrom( collisionSource, x, y, z );
}


void PickupBase::setup(const char *modelFile, const char *textureFile)
{
    ASSERT( model == NULL );
    model = new CCModelBase();
    model3d = CCModelObj::CacheModel( modelFile, textureFile );
    model->addModel( model3d );
    
    float modelWidth = MAX( model3d->getWidth(), model3d->getDepth() );
    float modelHeight = model3d->getHeight();
    {
        // Adjust model size
        const float modelSize = modelHeight > modelWidth ? modelHeight : modelWidth;
        const float scaleFactor = 10.0f / modelSize;
        CCVector3FillPtr( &model->scale, scaleFactor, scaleFactor, scaleFactor );
        
        modelWidth *= scaleFactor;
        modelHeight *= scaleFactor;
    }
    setSquareCollisionBounds( modelWidth, modelHeight );
    
    setPositionXYZ( 0.0f, collisionBounds.y, 0.0f );
    setRotationY( 180.0f );
}
