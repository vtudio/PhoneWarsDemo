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


CollideableFloor::CollideableFloor()
{
}


// ObjectBase
bool CollideableFloor::update(const CCTime &time)
{
    return false;
}


void CollideableFloor::renderModel(const bool alpha)
{
	super::renderModel( alpha );
}


bool CollideableFloor::shouldCollide(CCObjectCollideable *collideWith, const bool initialCall)
{
	return super::shouldCollide( collideWith, initialCall );
}


CCObjectCollideable* CollideableFloor::recieveCollisionFrom(CCObjectCollideable *collisionSource, const float x, const float y, const float z)
{
    // HACK: For now floor always stabilises objects
    if( collisionSource->getPosition().y != collisionSource->collisionBounds.y )
    {
        collisionSource->setPositionY( collisionSource->collisionBounds.y );
    }
    return NULL;
//	const float collidedWithY = collisionSource->positionPtr->y - collisionSource->collisionBounds.y;
//	if( collidedWithY > -CC_SMALLFLOAT )
//	{
//		collisionSource->translate( 0.0f, -collidedWithY, 0.0f );
//		return NULL;
//	}
//	
//	return super::recieveCollisionFrom( collisionSource, x, y, z );
}


void CollideableFloor::setup(const float width, const float length, const float y)
{
	setCollisionBounds( width, CC_SMALLFLOAT, length );
	translate( 0.0f, y-collisionBounds.y, 0.0f );
	
	primitive = new CCPrimitiveSquare();
	primitive->setupYFacing( collisionBounds.x * 2.0f, -CC_SMALLFLOAT, collisionBounds.z * 2.0f, true, 1.0f );
	
	model = new CCModelBase();
	model->addPrimitive( primitive );
	
	model->setColour( CCColour() );
	
	CCUpdateCollisions( this );
}
