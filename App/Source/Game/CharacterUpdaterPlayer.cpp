/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CharacterUpdaterPlayer.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"
#include "CCModelObj.h"


CharacterUpdaterPlayer::CharacterUpdaterPlayer(CCPathFinderNetwork *pathFinderNetwork)
{	
    if( pathFinderNetwork == NULL )
    {
        pathFinderNetwork = &gEngine->collisionManager.pathFinderNetwork;
    }
    this->pathFinderNetwork = pathFinderNetwork;
    
	player = NULL;
    
	setHealth( 100.0f, true );
    
	moving = false;
    distanceToPositionTarget = MAXFLOAT;
	movementMagnitude = 1.0f;
	
	shootingState = state_stopped;
    shootingBurstTimer = 0.0f;
	shootingTarget = NULL;
	
	anchorNode = NULL;
	findNewAnchor = false;
	
	path = NULL;
	pathAnchorNode = NULL;
	currentPath = -1;
    
    enabled = true;
}


void CharacterUpdaterPlayer::destruct()
{	
    if( shootingState != state_stopped )
	{
        stopShooting();   
    }
    
    DELETE_POINTER( path );
	
	super::destruct();
}


bool CharacterUpdaterPlayer::update(const float delta)
{
	movePlayer( delta );
	
	if( shootingState != state_stopped )
	{
        if( shootingState == state_shooting )
        {
            if( shootingTarget != NULL )
            {
                if( shootingTarget->isActive() == false )
                {
                    shootingTarget = NULL;
                }
                
                // Update our shoot target as we follow our target
                else if( shootingBurstTimer > 0.0f )
                {
                    shootingLocation = *shootingTarget->positionPtr;
                }
            }
            
            player->shootWeapon( shootingLocation );
            
            shootingBurstTimer += delta;
            if( shootingBurstTimer >= 1.0f )
            {
                shootingBurstTimer = 1.0f;
                shootingState = state_stopping;
                shootingTarget = NULL;
            }
        }
        else
        {
            shootingBurstTimer -= delta * 2.0f;
            if( shootingBurstTimer > 0.0f )
            {
//                    if( shootAudioUsingIndex != -1 )
//                    {
//                        gEngine->audioManager->setVolume( shootAudioSampleIndices[shootAudioUsingIndex], shootTimer );
//                    }
            }
            else
            {
                stopShooting();
            }
        }
	}
    
    return true;
}


void CharacterUpdaterPlayer::goToScan(const CCVector3 &target)
{
	CCObjectCollideable *hitObject = CCMovementOctreeCollisionCheck( player, *player->positionPtr, target );
    if( hitObject != NULL )
    {
        if( CCText::Equals( hitObject->getType(), "health" ) )
        {
            PickupBase *pickup = (PickupBase*)( hitObject );
            if( pickup != NULL )
            {
                hitObject = NULL;
            }
        }
    }
    
	if( hitObject == NULL )
	{
		DELETE_POINTER( path );
		goTo( target );
	}
	else
	{
		goToUsingPath( target );
	}
}

void CharacterUpdaterPlayer::goToUsingPath(const CCVector3 &target)
{	
	const CCPathFinderNetwork::PathNode *targetNode = pathFinderNetwork->findClosestNode( player, &target, false );
    if( targetNode == NULL )
    {
        DELETE_POINTER( path );
		goTo( target );
        return;
    }
	
	if( path == NULL )
	{
		findNewAnchor = true;
		path = new CCPathFinderNetwork::Path();
	}
	getClosestNode();
	
	pathAnchorNode = anchorNode;
    if( pathAnchorNode != NULL )
    {
        pathFinderNetwork->findPath( *path, pathAnchorNode, targetNode );
    }
    
	if( path->endDirection == 0 )
	{
        // See if we can head straight to our target
        CCObjectCollideable *hitObject = CCMovementOctreeCollisionCheck( player, *player->positionPtr, target );
        if( hitObject == NULL )
        {
            DELETE_POINTER( path );
        }
	}
	
	// Check if we can head straight to our first path
	else
	{		
		const int pathDirection = path->directions[0];
        if( pathAnchorNode != NULL && pathDirection < pathAnchorNode->numberOfConnections )
        {
            const CCPathFinderNetwork::PathNode::PathConnection *usingConnection = &pathAnchorNode->connections[pathDirection];
            const CCVector3 *pathTarget = &usingConnection->node->point;
            CCObjectCollideable *hitObject = CCMovementOctreeCollisionCheck( player, *player->positionPtr, *pathTarget );
            currentPath = hitObject == NULL ? 0 : -1;
        }
        else
        {
            currentPath = 0;
        }
	}
	
	goTo( target );
}


void CharacterUpdaterPlayer::goTo(const CCVector3 &target)
{
	moving = true;
	positionTarget = target;
    distanceToPositionTarget = MAXFLOAT;
	positionTarget.y += player->collisionBounds.y;
    if( positionTarget.y != player->collisionBounds.y )
    {
        positionTarget.y = player->collisionBounds.y;
    }
	player->movementDirection.z = -movementMagnitude;
}


void CharacterUpdaterPlayer::shoot(CCObjectCollideable *enemy)
{
    if( enemy != NULL )
    {
        shoot( enemy->getPosition(), enemy );
    }
}


void CharacterUpdaterPlayer::shoot(const CCVector3 &target, CCObjectCollideable *enemy)
{	
	if( shootingState != state_shooting )
	{
		shootingState = state_shooting;
	}
	shootingLocation = target;
	shootingLocation.y += player->collisionBounds.y;

	shootingBurstTimer = 0.0f;
    
	shootingTarget = enemy;
}


CCObjectCollideable* CharacterUpdaterPlayer::scanForEnemy(const CCVector3 &target)
{
    const float hScanArea = player->collisionBoundsLength.width * 2.0f;
    
	static CCVector3 min, max;
	min = target;
	min.x -= hScanArea;
	min.z -= hScanArea;
	
	max = target;
	max.x += hScanArea;
	max.z += hScanArea;

    return CCOctreeCollisionCheck( min, max, player, collision_character );
}


void CharacterUpdaterPlayer::stopShooting()
{
    shootingState = state_stopped;
    player->unAimWeapon();
}


void CharacterUpdaterPlayer::recieveCollisionFrom(CCObjectCollideable *collisionSource)
{
}


bool CharacterUpdaterPlayer::reportAttack(CCObject *attackedBy, const float force, const float damage, const float x, const float y, const float z)
{
    if( health > 0.0f )
	{
		health -= damage;
	}
    
	CCVector3 attackVelocity = CCVector3( x, y, z );
	attackVelocity.unitize();
	attackVelocity.mul( force * 0.1f );
	player->incrementAdditionalVelocity( attackVelocity.x, attackVelocity.y, attackVelocity.z );
	findNewAnchor = true;
    
    player->getGame()->registerAttack( attackedBy, player, damage );
    
	return false;
}


void CharacterUpdaterPlayer::registerKill(CCObjectCollideable *killed)
{
    if( onKillCallback != NULL )
    {
        onKillCallback->run();
    }
}


void CharacterUpdaterPlayer::findClosestAnchorNode()
{
	anchorNode = pathFinderNetwork->findClosestNode( player, player->positionPtr, true );
}


void CharacterUpdaterPlayer::movePlayer(const float delta)
{
	if( moving )
	{
		if( path != NULL )
		{
			if( currentPath == -1 )
			{
				const CCVector3 *target = &pathAnchorNode->point;
				movePlayerRotation( *target, delta );
				
				const float distance = CCVector3DistanceCheck2D( *player->positionPtr, *target, false );
				if( distance < 1.0f )
				{
					currentPath = 0;
				}
			}
			else
			{
                bool validPath = false;
                if( currentPath < path->endDirection )
                {
                    const int pathDirection = path->directions[currentPath];
                    if( pathDirection < pathAnchorNode->numberOfConnections )
                    {
                        const CCPathFinderNetwork::PathNode::PathConnection *usingConnection = &pathAnchorNode->connections[pathDirection];
                        const CCVector3 *target = &usingConnection->node->point;
                        if( target != NULL )
                        {
                            validPath = true;
                            
                            movePlayerRotation( *target, delta );
                            
                            const float distance = CCVector3DistanceCheck2D( *player->positionPtr, *target, false );
                            if( distance < 20.0f )
                            {
                                bool goToNext = distance < 0.1f + player->collisionBounds.y;
                                if( goToNext == false )
                                {
                                    const CCVector3 *testTarget = &positionTarget;
                                    if( currentPath+1 < path->endDirection )
                                    {
                                        const CCPathFinderNetwork::PathNode::PathConnection *nextConnection = &usingConnection->node->connections[path->directions[currentPath+1]];
                                        testTarget = &nextConnection->node->point;
                                        if( testTarget == NULL )
                                        {
                                            testTarget = &positionTarget;
                                        }
                                    }
                                    CCObjectCollideable *hitObject = CCMovementOctreeCollisionCheck( player, *player->positionPtr, *testTarget );
                                    goToNext = hitObject == NULL;
                                }
                                
                                if( goToNext )
                                {
                                    pathAnchorNode = usingConnection->node;
                                    anchorNode = pathAnchorNode;
                                    currentPath++;
                                    if( currentPath >= path->endDirection )
                                    {
                                        DELETE_POINTER( path );
                                    }
                                }
                            }
                        }
                    }
                }
                
                if( validPath == false )
                {
                    // Try again?
                    goTo( positionTarget );
                }
			}
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
                landPlayerOnTarget( distance, delta );
            }
		}
		
		player->dirtyModelMatrix();
	}
}


const bool CharacterUpdaterPlayer::landPlayerOnTarget(const float distance, const float delta)
{
    // If we're not zoning in on our target
    if( distanceToPositionTarget == MAXFLOAT )
    {
        // Rotate towards our target first if we're pointing away from our target
        const float angleTowardsTarget = CCAngleTowards( player->getPosition(), positionTarget );
        const float angleDistance = CCDistanceBetweenAngles( player->rotation.y, angleTowardsTarget );
        if( angleDistance > 15.0f )
        {
            movePlayerRotation( positionTarget, delta );
            player->movementDirection.z = 0.0f;
        }
        else
        {
            distanceToPositionTarget = distance;
        }
    }
    
    // Now we are 100% rotated to our target
    else
    {
        if( distance <= distanceToPositionTarget )
        {
            distanceToPositionTarget = distance;
            player->movementDirection.z = -0.2f;
        }
        
        // As soon as we move past our target position, we know we've hit it
        else
        {
            CCObjectCollideable *collidedWith = CCBasicOctreeCollisionCheck( player, positionTarget, true );
            if( collidedWith == NULL )
            {
                player->setPosition( positionTarget );
            }
            player->movementDirection.z = 0.0f;
            moving = false;
            return true;
        }
    }
    return false;
}


const float CharacterUpdaterPlayer::movePlayerRotation(const CCVector3 &target, const float delta)
{
	const float angleTowardsTarget = CCAngleTowards( player->getPosition(), target );
	CCToRotation( player->rotation.y, angleTowardsTarget, delta * 720.0f );
    return angleTowardsTarget;
}
