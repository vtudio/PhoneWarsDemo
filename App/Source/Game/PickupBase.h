/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : PickupBase.h
 * Description : An object that gets picked up
 *
 * Created     : 28/04/12
 *-----------------------------------------------------------
 */

class PickupBase : public CCObjectCollideable
{
public:
    typedef CCObjectCollideable super;

	PickupBase(const char *name, const char *type, const char *pickupID=NULL);

	// ObjectBase
	virtual bool update(const CCTime &time);
	virtual void renderModel(const bool alpha);

	// CollideableBase
	virtual bool shouldCollide(CCObjectCollideable *collideWith, const bool initialCall);
    
	// Ask the collidedWith object if we've collided
	virtual CCObjectCollideable* recieveCollisionFrom(CCObjectCollideable *collisionSource, const float x, const float y, const float z);
    
    void setup(const char *modelFile, const char *textureFile);
    
    const char* getName()
    {
        return name.buffer;
    }
    
    virtual const char* getType() const
    {
        return type.buffer;
    }
    
    virtual const char* getID() const
    {
        return pickupID.buffer;
    }

protected:
    CCText name, type;
    CCText pickupID;
    class CCModelObj *model3d;
};
