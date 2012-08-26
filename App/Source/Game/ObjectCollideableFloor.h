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

class CollideableFloor : public CCObjectCollideable
{
public:
    typedef CCObjectCollideable super;

	CollideableFloor();

	// ObjectBase
	virtual bool update(const CCTime &time);
	virtual void renderModel(const bool alpha);

	// CollideableBase
	virtual bool shouldCollide(CCObjectCollideable *collideWith, const bool initialCall);
    
	// Ask the collidedWith object if we've collided
	virtual CCObjectCollideable* recieveCollisionFrom(CCObjectCollideable *collisionSource, const float x, const float y, const float z);
	
	void setup(const float width, const float length, const float y=0.0f);

public:
	CCPrimitiveSquare *primitive;
};
