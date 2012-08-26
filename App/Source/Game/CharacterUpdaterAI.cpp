/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CharacterUpdaterAI.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"
#include "CCModelObj.h"


CharacterUpdaterAI::CharacterUpdaterAI(CCPathFinderNetwork *pathFinderNetwork) :
    CharacterUpdaterPlayer( pathFinderNetwork )
{	
	waypointTotal = waypointCurrent = 0;
	waypointCycle = cycle_off;
	targetConnection = NULL;
	
	nodesPerMovementCycle = 0;
	wait = 0.0f;
	
	enemy = NULL;
	scanForEnemyTimer = 0.0f;
	followingEnemy = false;
	distanceToEnemy = MAXFLOAT;
}


void CharacterUpdaterAI::destruct()
{	
	super::destruct();
}


bool CharacterUpdaterAI::update(const float delta)
{
    if( enabled )
    {
        if( enemy != NULL )
        {
            if( enemy->isActive() == false )
            {
                enemy = NULL;
            }
            else 
            {
                scanForEnemyTimer += delta;
                if( scanForEnemyTimer > 1.0f )
                {
                    scanForEnemyTimer -= scanForEnemyTimer;
                    
                    distanceToEnemy = CCVector3DistanceCheck2D( *player->positionPtr, *enemy->positionPtr );
                    if( distanceToEnemy < CC_SQUARE( 100.0f ) )
                    {
                        targetEnemy();
                    }
                    else if( followingEnemy && distanceToEnemy < CC_SQUARE( 100.0f ) )
                    {
                        targetEnemy();
                    }
                    else if( moving == false )
                    {
                        // Restart tracking if we've stopped moving
                        if( followingEnemy )
                        {
                            followingEnemy = false;
                        }
                        
                        {
                            if( waypointCycle == cycle_off )
                            {
                                if( waypointTotal > 0 )
                                {
                                    waypointTotal--;
                                    if( waypointTotal > 0 )
                                    {
                                        for( int i=0; i<waypointTotal; ++i )
                                        {
                                            waypoints[i] = waypoints[i+1];
                                        }
                                        goToUsingPath( waypoints[0] );
                                    }	
                                }
                            }
                            else
                            {
                                nextWaypoint();
                            }
                        }
                    }
                }
            }
        }
        
        if( shootingState == state_stopped )
        {
            if( followingEnemy )
            {
                if( enemy == NULL )
                {
                    followingEnemy = false;
                }
                else if( distanceToEnemy < CC_SQUARE( 50.0f ) )
                {
                    shoot( *enemy->positionPtr, enemy );
                }
            }
        }
    }
    
    return super::update( delta );
}


void CharacterUpdaterAI::movePlayer(const float delta)
{
	if( wait > 0.0f )
	{
		player->movementDirection.z = 0.0f;
		wait -= delta;
		
		if( wait <= 0.0f )
		{
			goTo( targetConnection->node->point );
		}
	}
	else if( moving )
	{	
		if( path != NULL )
		{
			super::movePlayer( delta );
		}
		else
		{
            const float distance = CCVector3DistanceCheck2D( player->getPosition(), positionTarget, false );
            if( distance > 5.0f )
            {
                movePlayerRotation( positionTarget, delta );
            }
            else
			{	
				if( waypointCycle == cycle_aiNodes && enabled && followingEnemy == false )
				{
                    if( pathAnchorNode == NULL || targetConnection == NULL )
                    {
                        getClosestNode();
                        pickRandomNode();
                    }
					else if( targetConnection->distance > 10.0f ||
                       distance < 0.1f )
					{
						pathAnchorNode = targetConnection->node;
						anchorNode = pathAnchorNode;
						
						if( nodesPerMovementCycle++ > 10 )
						{
							int shouldWait = rand() % 5;
							if( shouldWait == 0 )
							{
								wait = (float)( rand() % 15 ) + 5.0f;
							}
							else
							{
								movementMagnitude = ( ( rand() % 5 ) * 0.1f ) + 0.3f;
							}
							nodesPerMovementCycle = 0;
						}
						
						// Pick a path from that node
						int randomType = rand() % 5;
						if( randomType > 1 )
						{
							int closestConnection = 0;
							float closestAngle = MAXFLOAT;
							for( int i=0; i<pathAnchorNode->numberOfConnections; ++i )
							{
								const float angleDistance = CCDistanceBetweenAngles( player->rotation.y, pathAnchorNode->connections[i].angle );
								if( angleDistance < closestAngle )
								{
									closestConnection = i;
									closestAngle = angleDistance;
								}
							}
							
							if( pathAnchorNode->numberOfConnections > 0 )
							{
								targetConnection = &pathAnchorNode->connections[closestConnection];
								goTo( targetConnection->node->point );
							}
							else
							{
								
							}
						}
						else
						{
							pickRandomNode();
						}
					}
				}
				else
				{
                    if( landPlayerOnTarget( distance, delta ) )
                    {        
                        if( enabled && followingEnemy == false )
                        {
                            if( waypointCycle == cycle_off )
                            {
                                if( waypointTotal > 0 )
                                {
                                    waypointTotal--;
                                    if( waypointTotal > 0 )
                                    {
                                        for( int i=0; i<waypointTotal; ++i )
                                        {
                                            waypoints[i] = waypoints[i+1];
                                        }
                                        goToUsingPath( waypoints[0] );
                                    }	
                                }
                            }
                            else
                            {
                                nextWaypoint();
                            }
                        }
                    }
				}
			}
		}
	}
}


void CharacterUpdaterAI::recieveCollisionFrom(CCObjectCollideable *collisionSource)
{
    if( waypointCycle == cycle_random )
	{
		nextWaypoint();
	}
	else if( waypointCycle == cycle_aiNodes )
	{
		wait = 0.0f;
		pickRandomNode();
	}
	else if( waypointCycle == cycle_inOrder )
	{
		nextWaypoint();
	}
	else
	{
		waypointCycle = waypointCycle;
	}
}


bool CharacterUpdaterAI::reportAttack(CCObject *attackedBy, const float force, const float damage, const float x, const float y, const float z)
{
    super::reportAttack(attackedBy, force, damage, x, y, z );
    
    if( health <= 0.0f )
    {
        //player->triggerDeath( attackedBy, speed, x, y, z );
        return true;
    }
	return false;
}


void CharacterUpdaterAI::targetEnemy()
{
    goToUsingPath( *enemy->positionPtr );
    followingEnemy = true;
}


void CharacterUpdaterAI::setWaypointCycle(WaypointCycleType inCycleType)
{
	waypointCycle = inCycleType;
	
    if( enabled )
    {
        if( waypointCycle == cycle_random )
        {
            waypointCurrent = rand() % waypointTotal;
            goTo( waypoints[waypointCurrent] );
        }
        else if( waypointCycle == cycle_aiNodes )
        {
            getClosestNode();
            pickRandomNode();
        }
    }
}


void CharacterUpdaterAI::addWaypoint(CCVector3 target)
{
	waypoints[waypointTotal] = target;
	
	if( enabled && moving == false )
	{
		waypointCurrent = waypointTotal;
		goToUsingPath( target );
	}
	waypointTotal++;
	
	ASSERT( waypointTotal < max_waypoints );
}


void CharacterUpdaterAI::scanWaypoints(const float radius, const int amount)
{
	const CCVector3 *points[amount];
	const uint found = pathFinderNetwork->findClosestNodes( *player->positionPtr, radius, points, amount );
	for( uint i=0; i<found; ++i )
	{
		addWaypoint( *points[i] );
	}
}


void CharacterUpdaterAI::nextWaypoint()
{
	if( waypointCycle == cycle_inOrder )
	{
		waypointCurrent++;
		if( waypointCurrent >= waypointTotal )
		{
			waypointCurrent = 0;
		}
	}
	else if( waypointCycle == cycle_random )
	{
		waypointCurrent = rand() % waypointTotal;
	}
	
	goToUsingPath( waypoints[waypointCurrent] );
}


void CharacterUpdaterAI::pickRandomNode()
{
	if( anchorNode != NULL && anchorNode->numberOfConnections > 0 )
	{
		int randomPath = rand() % anchorNode->numberOfConnections;
		targetConnection = &anchorNode->connections[randomPath];
		goTo( targetConnection->node->point );
	}
}
