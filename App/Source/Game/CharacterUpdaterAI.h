/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CharacterUpdaterAI.h
 * Description : Update for our ai characters.
 *
 * Created     : 09/05/10
 *-----------------------------------------------------------
 */

#define DEBUG_SHOOTING

class CharacterUpdaterAI : public CharacterUpdaterPlayer
{
public:
    typedef CharacterUpdaterPlayer super;

	CharacterUpdaterAI(CCPathFinderNetwork *pathFinderNetwork=NULL);

	// BaseType
	virtual void destruct();

	// ControllerBase
	virtual bool update(const float delta);
    
	// ControllerPlayer
	virtual void movePlayer(const float delta);
    
	// Ask the collidedWith object if we've collided
	virtual void recieveCollisionFrom(CCObjectCollideable *collidedWith);
	
	virtual bool reportAttack(CCObject *attackedBy, const float force, const float damage, const float x, const float y, const float z);
    
	void setEnemy(CharacterPlayer *inEnemy) { enemy = inEnemy; }
    void targetEnemy();
    
	enum WaypointCycleType
	{
		cycle_off,
		cycle_inOrder,
		cycle_random,
		cycle_aiNodes
	};
	void setWaypointCycle(WaypointCycleType inCycleType);
	void addWaypoint(CCVector3 target);
	void scanWaypoints(const float radius, const int amount=max_waypoints);

protected:
	void nextWaypoint();
	void pickRandomNode();
    
	enum { max_waypoints = 50 };
	CCVector3 waypoints[max_waypoints];
	int waypointCurrent, waypointTotal;
    
	WaypointCycleType waypointCycle;
	const CCPathFinderNetwork::PathNode::PathConnection *targetConnection;
    
	int nodesPerMovementCycle;
	float wait;
    
	CharacterPlayer *enemy;
	float scanForEnemyTimer;
	bool followingEnemy;
	float distanceToEnemy;
};
