/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CharacterBase.h
 * Description : Base class for our character.
 *
 * Created     : 09/09/09
 *-----------------------------------------------------------
 */

class CharacterBase : public CCObjectMoveable
{
public:
    typedef CCObjectMoveable super;

	CharacterBase();
	virtual void destruct();
	
	// MoveableBase
	virtual void updateMovement(const float delta);
	virtual float applyMovementDirection(const float delta);
	virtual void applyVelocity(const float delta, const float directionMagnitude);
	virtual void reportVerticalCollision(const CCObjectCollideable *collidedWith);
	
	virtual void updateMovementDirection(CCVector3 *deltaVector);
};
