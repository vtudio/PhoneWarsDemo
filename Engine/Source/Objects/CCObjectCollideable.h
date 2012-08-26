/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCObjectCollideable.h
 * Description : A scene managed collideable object.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

class CCObjectCollideable : public CCObject
{
public:
	typedef CCObject super;

	CCObjectCollideable();
	virtual void destruct();
	
	// CCRenderable
	virtual void setPositionXYZ(const float x, const float y, const float z);
	virtual void translate(const float x, const float y, const float z);
	
	// CCObject
	virtual void setScene(CCSceneBase *scene);
	virtual void removeFromScene();
	virtual void deactivate();
    virtual bool shouldCollide(CCObjectCollideable *collideWith, const bool initialCall);
    
protected:
    virtual void renderModel(const bool alpha);
	virtual void renderCollisionBox();
	
public:
    virtual const char* getType() const { return NULL; }
    
    void setSquareCollisionBounds(const float size);
    void setSquareCollisionBounds(const float width, const float heigth);
	void setHSquareCollisionBounds(const float hSize);
	void setHSquareCollisionBounds(const float hWidth, const float hHeight);
    
    void setCollisionBounds(const float width, const float height, const float depth);
	void setHCollisionBounds(const float hWidth, const float hHeight, const float hDepth);
	
	// Ask to report a collision to the collidedWith object
	virtual CCObjectCollideable* requestCollisionWith(CCObjectCollideable *collidedWith);
	
	// Ask the collidedWith object if we've collided
	virtual CCObjectCollideable* recieveCollisionFrom(CCObjectCollideable *collisionSource, const float x, const float y, const float z);
	
    virtual bool reportAttack(CCObject *attackedBy, const float force, const float damage, const float x, const float y, const float z);

    virtual bool isMoveable() { return false; }

	virtual void ownObject(CCObjectCollideable *object);
	virtual void unOwnObject(CCObjectCollideable *object);

protected:
	void setOwner(CCObjectCollideable *newOwner);
	void removeOwner(CCObjectCollideable *currentOwner);
    
public:
    inline const char* getDebugName()
    {  
#ifdef DEBUGON
        return debugName.buffer;
#endif
        return "";
    }
    
    inline void setDebugName(const char *name)
    {
#ifdef DEBUGON
        debugName = name;
#endif
    }

public:
	uint collideableType;
	CCVector3 collisionBounds;
    CCSize collisionBoundsLength;
    CCSize inverseBoundsLength;

	CCVector3 min, max;
	bool updateCollisions;

	CCList<CCOctree> octrees;

	uint drawOrder;
    bool visible;

protected:
	// Since parenting disables collisions, the owner system leaves our owned objects alone, but informs us when an event occurs
	CCObjectCollideable *owner;
    CCList<CCObjectCollideable> owns;
    
#ifdef DEBUGON
    CCText debugName;
#endif
};
