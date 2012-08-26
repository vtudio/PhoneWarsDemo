/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CharacterPlayer.h
 * Description : Player class for our character.
 *
 * Created     : 01/05/10
 *-----------------------------------------------------------
 */

class CharacterUpdaterPlayer;
class CCModelObj;

class CharacterPlayer : public CharacterBase
{
public:
    typedef CharacterBase super;

    CharacterPlayer(const float inMovementSpeed=50.0f);
    
    void setupGame(class SceneGameSyndicate *sceneGame, const char *playerID);
    
    SceneGameSyndicate* getGame() { return sceneGame; }
    const char* getType() const { return type.buffer; }
    
    virtual void setupAI(CharacterUpdaterPlayer *controller);
    
    void setupBody(const char *modelFile, const char *textureFile);
    virtual void setupHead(const char *modelFile, const char *textureFile);
    
    // Set the player size and orientation
    virtual void finalizeModel();
    
    void setupWeapon(CCModelObj *modelObj);
    const bool useableWeapon(const char *weaponName);
    
protected:
    CCModelObj* setupModel(const char *modelFile, const char *textureFile);
    
public:
    void setupCharacterIndicator(const char *textureFile, CCColour colour);
    void setupShootingIndicator(const char *textureFile, CCColour colour);
    void scaleShootingIndicator(const float target);

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
    
    const char* getPlayerID() const { return playerID.buffer; }
    void activateHeadAnimation()
    {
        modelHeadAnimationActive = true;
    }

	virtual const bool aimWeapon(const CCVector3 &target);
protected:
    virtual void updateWeaponAim(const float timeDelta);
public:
    virtual void shootWeapon(const CCVector3 &target);
	void unAimWeapon();

	void triggerDeath(CCObject *attackedBy, const float force, const float x, const float y, const float z);
	void animatedDeath();

// Parameters
public:
	CharacterUpdaterPlayer *controller;

protected:
    SceneGameSyndicate *sceneGame;
    CCText playerID;
    CCText type;
    
    CCModelObj *modelBody, *modelHead;
    WeaponBase *weapon;
    
    bool modelRotationTargetActive;
    float modelRotationTarget;
    
    bool modelHeadAnimationActive;
    uint modelHeadAnimationState;
    
    CCObject *characterIndicator;
    CCObject *shootingIndicator;
    CCInterpolatorV3<CCInterpolatorSin2Curve> shootingScaleInterpolator;
    
    float lastTimeAttacked;
	bool deathFadeOut;
};
