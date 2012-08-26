/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCObjectCollideable.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCObjects.h"
#include "CCSceneBase.h"
#include "CCOctree.h"
#include "CCTextureManager.h"


CCObjectCollideable::CCObjectCollideable()
{
    octreeRender = true;

	collideableType = 0;
	setHCollisionBounds( 0.5f, 0.5f, 0.5f );

    updateCollisions = true;
	octrees.allocate( 32 );
	
	drawOrder = 100;
    visible = false;
	
	owner = NULL;
}


void CCObjectCollideable::destruct()
{	
	super::destruct();
}


// CCRenderable
void CCObjectCollideable::setPositionXYZ(const float x, const float y, const float z)
{
	super::setPositionXYZ( x, y, z );
    updateCollisions = true;
    CCOctreeRefreshObject( this );
}


void CCObjectCollideable::translate(const float x, const float y, const float z)
{
	super::translate( x, y, z );
    updateCollisions = true;
    CCOctreeRefreshObject( this );
}


// CCObject
void CCObjectCollideable::setScene(CCSceneBase *scene)
{
	super::setScene( scene );
	CCAddFlag( collideableType, collision_box );
    scene->addCollideable( this );
}


void CCObjectCollideable::removeFromScene()
{	
    inScene->removeCollideable( this );
	super::removeFromScene();
}


void CCObjectCollideable::deactivate()
{
	super::deactivate();
	CCRemoveFlag( collideableType, collision_box );
	CCOctreeRemoveObject( this );
    
    if( owner != NULL )
	{
		owner->unOwnObject( this );
		owner = NULL;
	}
	
	for( int i=0; i<owns.length; ++i )
	{
		owns.list[i]->removeOwner( this );
	}
	owns.freeList();
}


bool CCObjectCollideable::shouldCollide(CCObjectCollideable *collideWith, const bool initialCall)
{
	// Ask myself if we should collide
    if( super::shouldCollide( collideWith, initialCall ) == false )
    {
        return false;
	}
	
	// Ask my owner if I should collide with this object
	if( owner != NULL && owner != parent )
    {
        if( owner->shouldCollide( collideWith, initialCall ) == false )
        {
            return false;
		}
	}
	
	// Ask the other object if we should collide
	if( initialCall )
    {
        return collideWith->shouldCollide( this, false );
	}
	
    // Yeah let's collide baby
    return true;
}


void CCObjectCollideable::renderModel(const bool alpha)
{
	super::renderModel( alpha );
	
	if( alpha == transparent &&
        gEngine->renderFlags & render_collisionBoxes &&
        CCHasFlag( collideableType, collision_box ) )
	{
		if( transparent == false )
		{
			GLEnableBlend();
		}
		
		renderCollisionBox();
		
		if( transparent == false )
		{
			GLDisableBlend();
		}
	}
}


void CCObjectCollideable::renderCollisionBox()
{
	GLPushMatrix();
	{
        glLineWidth( LINE_WIDTH );
        if( scale != NULL )
        {
            GLScalef( 1.0f / scale->x, 1.0f / scale->y, 1.0f / scale->z );
        }
		GLScalef( collisionBounds.x, collisionBounds.y, collisionBounds.z );
		//glColor4f( 1.0f, 1.0f, 0.0f, 0.5f );
        //RenderCube( true );
		
		gEngine->textureManager->setTextureIndex( 0 );
		GLRotatef( -rotation.y, 0.0f, 1.0f, 0.0f );
        static CCColour colour( 1.0f, 0.0f, 0.0f, 0.5f );
		CCSetColour( colour );
        CCRenderCube( true );
	}
	GLPopMatrix();
}


void CCObjectCollideable::setSquareCollisionBounds(const float size)
{
    setCollisionBounds( size, size, size );
}


void CCObjectCollideable::setSquareCollisionBounds(const float width, const float height)
{
    setCollisionBounds( width, height, width );
}



void CCObjectCollideable::setHSquareCollisionBounds(const float hSize)
{
	setHCollisionBounds( hSize, hSize, hSize );
}


void CCObjectCollideable::setHSquareCollisionBounds(const float hWidth, const float hHeight)
{
	setHCollisionBounds( hWidth, hHeight, hWidth );
}


void CCObjectCollideable::setCollisionBounds(const float width, const float height, const float depth)
{
    setHCollisionBounds( width * 0.5f, height * 0.5f, depth * 0.5f );
}


void CCObjectCollideable::setHCollisionBounds(const float hWidth, const float hHeight, const float hDepth)
{
	collisionBounds.x = hWidth;
	collisionBounds.y = hHeight;
	collisionBounds.z = hDepth;
    
	collisionBoundsLength.width = hWidth > hDepth ? hWidth : hDepth;
    collisionBoundsLength.width *= 2.0f;
    collisionBoundsLength.height = hHeight * 2.0f;
	inverseBoundsLength.width = 1.0f / collisionBoundsLength.width;
	inverseBoundsLength.height = 1.0f / collisionBoundsLength.height;
    
    updateCollisions = true;
}


CCObjectCollideable* CCObjectCollideable::requestCollisionWith(CCObjectCollideable *collidedWith)
{
	return collidedWith->recieveCollisionFrom( this, 0.0f, 0.0f, 0.0f );
}


CCObjectCollideable* CCObjectCollideable::recieveCollisionFrom(CCObjectCollideable *collisionSource, const float x, const float y, const float z)
{	
	return this;
}


bool CCObjectCollideable::reportAttack(CCObject *attackedBy, const float force, const float damage, const float x, const float y, const float z)
{
    return false;
}


void CCObjectCollideable::ownObject(CCObjectCollideable *object)
{
	owns.add( object );
	object->setOwner( this );
}


void CCObjectCollideable::unOwnObject(CCObjectCollideable *object)
{
	if( owns.remove( object ) )
	{
		if( owns.length == 0 )
		{
			owns.freeList();
		}
	}
}


void CCObjectCollideable::setOwner(CCObjectCollideable *newOwner)
{
	owner = newOwner;
}


void CCObjectCollideable::removeOwner(CCObjectCollideable *currentOwner)
{
	if( currentOwner == owner )
	{
		owner = NULL;
	}
}
