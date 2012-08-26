/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : ObjectMoveableBullet.h
 * Description : A bullet that hits other characters.
 *
 * Created     : 02/05/10
 *-----------------------------------------------------------
 */

class ObjectMoveableBullet : public CCObjectMoveable
{
public:
    typedef CCObjectMoveable super;

	ObjectMoveableBullet(const float inLife=1.0f);

	// ObjectBase
	virtual bool update(const CCTime &time);
    virtual void renderModel(const bool alpha);

	// CCObjectCollideable
    
    // Ask to report a collision to the collidedWith object
	virtual CCObjectCollideable* requestCollisionWith(CCObjectCollideable *collidedWith);
	
	// Ask the collidedWith object if we've collided
	virtual CCObjectCollideable* recieveCollisionFrom(CCObjectCollideable *collisionSource, const float x, const float y, const float z);
	
	virtual bool reportAttack(CCObject *attackedBy, const float force, const float damage, const float x, const float y, const float z);
    
    bool attackObject(CCObjectCollideable *victim);

protected:
	float life;
};