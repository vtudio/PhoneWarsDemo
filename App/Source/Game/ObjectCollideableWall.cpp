/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CollideableFloor.h
 * Description : A floor object.
 *
 * Created     : 12/09/09
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"


CollideableWall::CollideableWall()
{
    primitive = NULL;
}


// CCRenderable
void CollideableWall::setPositionXYZ(const float x, const float y, const float z)
{
	super::setPositionXYZ( x, y, z );
	translate( offsetX, 0.0f, offsetZ );
    CCOctreeRefreshObject( this );
}


// ObjectBase
void CollideableWall::renderModel(const bool alpha)
{
	super::renderModel( alpha );
}


// CollideableBase
bool CollideableWall::shouldCollide(CCObjectCollideable *collideWith, const bool initialCall)
{	
	return super::shouldCollide( collideWith, initialCall );
}


void CollideableWall::setup(CCSceneBase *scene, const float x, const float z, const float size, const float height)
{
	offsetX = x;
	offsetZ = z;
	
	const float collisionSize = CC_SMALLFLOAT;
    const float hSize = size * 0.5f;
	if( x != 0.0f )
	{
		setHCollisionBounds( collisionSize, height, hSize );
	}
	else
	{
		setHCollisionBounds( hSize, height, collisionSize );
	}
	
	translate( x, collisionBounds.y, z );
	
	if( scene != NULL )
	{
		setScene( scene );
	}
}

	
void CollideableWall::createModel()
{
    model = new CCModelBase();
    CCPrimitiveSquare *primitive = new CCPrimitiveSquare();
    
    if( collisionBounds.z > collisionBounds.x )
    {
        primitive->setupXFacing( 0.0f, collisionBounds.y, collisionBounds.z );
    }
    else
    {
        primitive->setupZFacing( collisionBounds.x, collisionBounds.y, 0.0f );
    }

    model->translate( 0.0f, -collisionBounds.y, 0.0f );
    model->addPrimitive( primitive );
    model->setColour( CCColour() );
}
