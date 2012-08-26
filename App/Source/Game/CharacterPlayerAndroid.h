/*-----------------------------------------------------------
 * Copyright © 2011 - 2012 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CharacterPlayerAndroid.h
 * Description : Player class for our Android character.
 *
 * Created     : 06/07/12
 *-----------------------------------------------------------
 */

class CharacterUpdaterPlayer;
class CCModelObj;

class CharacterPlayerAndroid : public CharacterPlayer
{
public:
    typedef CharacterPlayer super;
    
    static CharacterPlayerAndroid* Spawn(const char *type);

    CharacterPlayerAndroid(const float inMovementSpeed=50.0f);
    
    void setupScreen(const char *modelFile, const char *textureFile);
    void setupLeftArm(const char *modelFile, const char *textureFile);
    void setupRightArm(const char *modelFile, const char *textureFile);
    
    // Set the player size and orientation
    virtual void finalizeModel();

    void setupWeapon2(CCModelObj *modelObj);

public:
	// ObjectBase
	virtual bool update(const CCTime &time);
    
protected:
    virtual void renderModel(const bool alpha);

public:
	// CollideableBase
	// Ask to report a collision to the collidedWith object
	virtual CCObjectCollideable* requestCollisionWith(CCObjectCollideable *collidedWith);
	
	// Ask the collidedWith object if we've collided
	virtual CCObjectCollideable* recieveCollisionFrom(CCObjectCollideable *collisionSource, const float x, const float y, const float z);
    
	virtual bool reportAttack(CCObject *attackedBy, const float force, const float damage, const float x, const float y, const float z);

protected:
    virtual void updateWeaponAim(const float timeDelta);
public:
    virtual void shootWeapon(const CCVector3 &target);
    
protected:
    CCModelObj *modelScreen;
    CCModelObj *modelLeftArm, *modelRightArm;
    
    WeaponBase *weapon2;
    float weaponSwitchTimeOut;
};
