/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CharacterBase.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"


CharacterBase::CharacterBase()
{
}


void CharacterBase::destruct()
{	
	super::destruct();
}


// MoveableBase
void CharacterBase::updateMovement(const float delta)
{	
	super::updateMovement( delta );
}


float CharacterBase::applyMovementDirection(const float delta)
{
	return super::applyMovementDirection( delta );
}


void CharacterBase::applyVelocity(const float delta, const float movementMagnitude)
{	
	super::applyVelocity( delta, movementMagnitude );
}


void CharacterBase::reportVerticalCollision(const CCObjectCollideable *collidedWith)
{	
	super::reportVerticalCollision( collidedWith );
}


void CharacterBase::updateMovementDirection(CCVector3 *deltaVector)
{
	movementDirection = *deltaVector;
}



