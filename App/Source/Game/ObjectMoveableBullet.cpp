/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : ObjectMoveableBullet.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"


ObjectMoveableBullet::ObjectMoveableBullet(const float inLife)
{
    setDebugName( "Bullet" );
    
	movementSpeed = 200.0f;
	gravity = false;
	life = inLife;
}


// ObjectBase
bool ObjectMoveableBullet::update(const CCTime &time)
{
	if( life <= 0.0f )
	{
		deleteLater();
	}
	else
	{	
		life -= time.delta;
		super::update( time );
        
//        if( model != NULL )
//        {
//            if( model->getColour != NULL )
//            {
//                CCToTarget( model->colour->alpha, 1.0f, gameTime.delta * 2.0f );
//            }
//        }
	}
    return true;
}


void ObjectMoveableBullet::renderModel(const bool alpha)
{
    if( alpha )
    {
        super::renderModel( alpha );
    }
    else
    {
        super::renderModel( alpha );
    }
}


// Ask to report a collision to the collidedWith object
CCObjectCollideable* ObjectMoveableBullet::requestCollisionWith(CCObjectCollideable *collidedWith)
{
    if( attackObject( collidedWith ) )
    {
        return super::requestCollisionWith( collidedWith );
    }
    return NULL;
}


// Ask the collidedWith object if we've collided
CCObjectCollideable* ObjectMoveableBullet::recieveCollisionFrom(CCObjectCollideable *collisionSource, const float x, const float y, const float z)
{
    if( attackObject( collisionSource ) )
    {
        return super::recieveCollisionFrom( collisionSource, x, y, z );
    }
    return NULL;
}


bool ObjectMoveableBullet::reportAttack(CCObject *attackedBy, const float force, const float damage, const float x, const float y, const float z)
{
    return false;
}


bool ObjectMoveableBullet::attackObject(CCObjectCollideable *victim)
{
    if( victim->shouldRender )
    {
        if( victim->reportAttack( owner, movementSpeed, 5.0f, velocity.x, velocity.y, velocity.z ) )
        {
            // Killed someone callback..
        }
        
        life = 0.0f;
        return true;
    }
    return false;
}
