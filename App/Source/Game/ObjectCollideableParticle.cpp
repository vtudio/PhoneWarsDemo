/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : ObjectCollideableParticle.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"


ObjectCollideableParticle::ObjectCollideableParticle(const float inLife)
{
    setDebugName( "Particle" );
    
	life = inLife;
    
    setTransparent();
    readDepth = false;
    writeDepth = false;
    
    drawOrder = 201;
}


// ObjectBase
bool ObjectCollideableParticle::update(const CCTime &time)
{
	if( life <= 0.0f )
	{
		deleteLater();
	}
	else
	{	
		life -= time.delta;
		super::update( time );
        
        translate( 0.0f, time.delta * 50.0f, 0.0f );
	}
    return true;
}


void ObjectCollideableParticle::renderModel(const bool alpha)
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