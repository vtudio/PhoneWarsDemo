/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : ObjectIndicator.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"


ObjectIndicator::ObjectIndicator(CCObjectCollideable *parentObject, const char *textureFile)
{
    model = new CCModelBase();
    
    CCPrimitiveSquare *primitive = new CCPrimitiveSquare();
    primitive->setupYFacing( 1.0f,
                             0.0f,
                             1.0f, true, 1.0f );
    setWidth( parentObject->collisionBoundsLength.width );
    
    model->addPrimitive( primitive );
    parentObject->addChild( this );
    
    setTransparent();
    readDepth = true;
    writeDepth = false;
    model->setColourAlpha( 0.5f );
    
    if( textureFile != NULL )
    {
        primitive->setTexture( textureFile, Resource_Packaged, true, false, false );
    }
}


ObjectIndicator::ObjectIndicator(CCSceneBase *parentScene, const char *textureFile)
{
    model = new CCModelBase();

    CCPrimitiveSquare *primitive = new CCPrimitiveSquare();
    primitive->setupYFacing( 1.0f,
                             0.0f,
                             1.0f, true, 1.0f );

    model->addPrimitive( primitive );
    setScene( parentScene );

    setTransparent();
    readDepth = true;
    writeDepth = false;
    model->setColourAlpha( 0.5f );

    if( textureFile != NULL )
    {
        primitive->setTexture( textureFile, Resource_Packaged, true, false, false );
    }
}


void ObjectIndicator::renderModel(const bool alpha)
{
    super::renderModel( alpha );
}


void ObjectIndicator::setWidth(const float width)
{
    CCVector3FillPtr( &model->scale, width );
}
