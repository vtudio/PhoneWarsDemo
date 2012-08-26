/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CharacterUpdaterPlayer.h
 * Description : Update for our player character.
 *
 * Created     : 02/05/10
 *-----------------------------------------------------------
 */

#define DEBUG_SHOOTING

class CharacterUpdaterPlayer : public CCUpdater
{
public:
    typedef CCUpdater super;

	CharacterUpdaterPlayer(CCPathFinderNetwork *pathFinderNetwork=NULL);

	// BaseType
	virtual void destruct();

	// ControllerBase
	virtual bool update(const float delta);

    inline void enable(const bool toggle) { enabled = toggle; }
	inline void setPlayer(CharacterPlayer *inPlayer) { player = inPlayer; }
    
    const float getHealth() { return health; }
    const float getHealthRatio() { return health/maxHealth; }
	void setHealth(const float inHealth, const bool initialize) 
    {
        health = inHealth; 
        if( initialize )
        {
            maxHealth = health;
        }
    }
    
    void resetHealth() { health = maxHealth; }

	void setMovementMagnitude(const float magnitude) { movementMagnitude = magnitude; }

	void goToScan(const CCVector3 &target);
	void goToUsingPath(const CCVector3 &target);
	void goTo(const CCVector3 &target);

	inline const CCPathFinderNetwork::PathNode* getClosestNode(const bool refresh=true) 
	{
		if( refresh || findNewAnchor || anchorNode == NULL )
		{
			findClosestAnchorNode();
		}
		return anchorNode; 
	}
    
	void shoot(CCObjectCollideable *enemy);
	void shoot(const CCVector3 &target, CCObjectCollideable *enemy=NULL);
    CCObjectCollideable* scanForEnemy(const CCVector3 &target);
    void stopShooting();
    
	// Ask the collidedWith object if we've collided
	virtual void recieveCollisionFrom(CCObjectCollideable *collisionSource);
	
	virtual bool reportAttack(CCObject *attackedBy, const float force, const float damage, const float x, const float y, const float z);

	void registerKill(CCObjectCollideable *killed);

protected:
	void findClosestAnchorNode();

	virtual void movePlayer(const float delta);
    virtual const bool landPlayerOnTarget(const float distance, const float delta);
	virtual const float movePlayerRotation(const CCVector3 &target, const float delta);

public:
    CCLambdaCallback *onKillCallback;

protected:
    bool enabled;
    
    CCPathFinderNetwork *pathFinderNetwork;
	CharacterPlayer *player;
    
	float health, maxHealth;
	
	bool moving;
	CCVector3 positionTarget;
    float distanceToPositionTarget;
	float movementMagnitude;

	enum ShootingState
	{
		state_shooting,
		state_stopping,
		state_stopped
	};
	ShootingState shootingState;
    float shootingBurstTimer;
	CCVector3 shootingLocation;
	CCObjectCollideable *shootingTarget;

	const CCPathFinderNetwork::PathNode *anchorNode;
	bool findNewAnchor;

	CCPathFinderNetwork::Path *path;
	const CCPathFinderNetwork::PathNode *pathAnchorNode;
	int currentPath;

#ifdef DEBUG_SHOOTING
	CCVector3 lastTarget;
#endif
};
