/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCObjectMoveable.h
 * Description : A scene managed moveable object.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

class CCObjectMoveable : public CCObjectCollideable
{
public:
	typedef CCObjectCollideable super;

	CCObjectMoveable();
	
	// CCObject
    virtual bool update(const CCTime &time);
	
	// CCObjectCollideable
	virtual CCObjectCollideable* requestCollisionWith(CCObjectCollideable *collidedWith);
    virtual bool isMoveable() { return true; }
	
	virtual void updateMovement(const float delta);
    virtual float applyMovementDirection(const float delta);
	
	virtual void applyVelocity(const float delta, const float directionMagnitude);
	float getCollisionPosition(float *thisObjectPosition, float *thisObjectBounds, float *collidedObjectPosition, float *collidedObjectBounds);
	CCObjectCollideable* applyHorizontalVelocity(const float velocityX, const float velocityZ);
	CCObjectCollideable* applyVerticalVelocity(const float increment);
	
	virtual void reportVerticalCollision(const CCObjectCollideable *collidedWith);

	static void setGravityForce(const float force) { gravityForce = force; }

	inline void setVelocity(const float x, const float y, const float z) { movementVelocity.x = x; movementVelocity.y = y; movementVelocity.z = z; }
	inline void setAdditionalVelocity(const float x, const float y, const float z) { additionalVelocity.x = x; additionalVelocity.y = y; additionalVelocity.z = z; }
	inline void incrementAdditionalVelocity(const float x, const float y, const float z) { additionalVelocity.x += x; additionalVelocity.y += y; additionalVelocity.z += z; }

	bool moveable;
	float movementSpeed;
	CCVector3 movementDirection;

protected:
	CCVector3 velocity;
	CCVector3 movementVelocity;
	CCVector3 additionalVelocity;
	float decelerationSpeed;
	
	bool gravity;
	static float gravityForce;
};
