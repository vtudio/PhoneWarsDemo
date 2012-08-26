/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : WeaponBase.h
 * Description : A weapon a character carries
 *
 * Created     : 03/05/12
 *-----------------------------------------------------------
 */

class CharacterPlayer;

class WeaponBase : public CCObject, public virtual CCLazyCallback
{
public:
    typedef CCObject super;

	WeaponBase(CharacterPlayer *character);
    void setup(CCModelObj *modelObj);
    virtual void destruct();
    
    const bool ready();
    void shootWeapon(const float characterRotation);
protected:
    void setupSpawnPoint(CCObjectCollideable *projectile, const float characterRotation);
    
public:
    void setFireRate(const float rate) { fireRate = rate; }
    void updateParentRotation(const float rotation);
    
    virtual bool update(const CCTime &time);
    virtual void renderObject(const CCCameraBase *camera, const bool alpha);
protected:
    virtual void renderModel(const bool alpha);
    
public:
    void shoot();

protected:
    CharacterPlayer *character;

    CCModelObj *modelWeapon;
    CCVector3 modelWeaponHomePosition;
    CCInterpolatorV3<CCInterpolatorSin2Curve> modelWeaponMovementInterpolator;
    
    CCObject *objectMuzzleFlash;
    CCModelObj *modelMuzzleFlash;
    
    CCVector3 spawnPoint;
    
    float fireRate;
    float shootingTimer;
    float shootingTimeSinceLastShot;
};
