/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCTile3D.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCObjects.h"
#include "CCTextureFontPage.h"
#include "CCControllerMovement.h"


CCTile3D::CCTile3D()
{
    setDebugName( "Tile3D" );
}


void CCTile3D::destruct()
{
    // Delete objects from CCTouchable
    onPress.deleteObjectsAndList();
    onMove.deleteObjectsAndList();
    onRelease.deleteObjectsAndList();
    onLoss.deleteObjectsAndList();
    
    super::destruct();
}

// CCRenderable
void CCTile3D::dirtyModelMatrix()
{
    super::dirtyModelMatrix();
    for( int i=0; i<attachments.length; ++i )
    {
        CCObject *attachment = attachments.list[i];
        attachment->dirtyModelMatrix();
    }
}


void CCTile3D::setPositionXYZ(const float x, const float y, const float z)
{
    if( position.x != x || position.y != y || position.z != z )
    {
        CCVector3 distance = position;
        super::setPositionXYZ( x, y, z );
        distance.sub( position );

        for( int i=0; i<attachments.length; ++i )
        {
            CCObject *attachment = attachments.list[i];
            attachment->translate( -distance.x, -distance.y, -distance.z );
        }

        CCOctreeRefreshObject( this );
    }
}


void CCTile3D::translate(const float x, const float y, const float z)
{
    super::translate( x, y, z );

    for( int i=0; i<attachments.length; ++i )
    {
        CCObject *attachment = attachments.list[i];
        attachment->translate( x, y, z );
    }

    CCOctreeRefreshObject( this );
}


// Positioning Tiles
void CCTile3D::positionTileY(float &y)
{
    y -= collisionBounds.y;
    translate( 0.0f, y, 0.0f );
    y -= collisionBounds.y;
}


void CCTile3D::positionTileBelow(CCTile3D *fromTile)
{
    setPosition( fromTile->getPosition() );
    translate( 0.0f, -( fromTile->collisionBounds.y + collisionBounds.y ), 0.0f );
}


void CCTile3D::positionTileAbove(CCTile3D *fromTile)
{
    setPosition( fromTile->getPosition() );
    translate( 0.0f, fromTile->collisionBounds.y + collisionBounds.y, 0.0f );
}


void CCTile3D::positionTileRight(CCTile3D *fromTile)
{
    setPosition( fromTile->getPosition() );
    translate( fromTile->collisionBounds.x + collisionBounds.x, 0.0f, 0.0f );
}


void CCTile3D::positionTileLeft(CCTile3D *fromTile)
{
    setPosition( fromTile->getPosition() );
    translate( -( fromTile->collisionBounds.x + collisionBounds.x ), 0.0f, 0.0f );
}