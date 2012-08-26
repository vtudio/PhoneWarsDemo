/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCCollisionTools.h
 * Description : Functionality for testing collision.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCCOLLISIONTOOLS_H__
#define __CCCOLLISIONTOOLS_H__


class CCObjectCollideable;
typedef struct CCOctree CCOctree;
#include "CCPathFinderNetwork.h"

struct CCCollisionManager
{
	CCCollisionManager(const float octreeSize);
	~CCCollisionManager();
	
	CCOctree *tree;
	float pruneTreesTimer;
    
    CCList<CCObjectCollideable> collideables;
	CCPathFinderNetwork pathFinderNetwork;
};



enum CCCollisionFlags
{
    collision_none          = 0x000000001,  // No collision
    collision_box           = 0x000000002,  // Anything with a collision box
    collision_static        = 0x000000004,  // Non-moveable objects
    collision_moveable      = 0x000000008,  // Moveable objects
    collision_character     = 0x000000010,  // Characters
    collision_ui            = 0x000000020   // UI
};



// Update the bounding boxes when an object is flagged
extern void CCUpdateCollisions(CCObjectCollideable *collideable, const bool dependantOnFlags=true);

// Collision test bounding boxes
extern bool CCBasicBoxCollisionCheck(const CCVector3 &sourceMin, const CCVector3 &sourceMax,
                                     const CCVector3 &targetMin, const CCVector3 &targetMax);

extern CCObjectCollideable* CCBasicCollisionCheck(CCObjectCollideable *sourceObject, const CCVector3 &targetLocation);
extern CCObjectCollideable* CCBasicOctreeCollisionCheck(CCObjectCollideable *sourceObject, 
                                                       const CCVector3 &targetLocation,
                                                       const bool requestCollisions=false,
                                                       const CCCollisionFlags flags=collision_box);

extern CCObjectCollideable* CCOctreeCollisionCheck(const CCVector3 &min, const CCVector3 &max, 
                                                  const CCObjectCollideable *sourceObject=NULL, 
                                                  const uint flags=collision_box);
extern CCObjectCollideable* CCMovementOctreeCollisionCheck(CCObjectCollideable *sourceObject, CCVector3 currentPosition, const CCVector3 &targetPosition);

// Collision test a ray
extern CCObjectCollideable* CCBasicLineCollisionCheck(CCObjectCollideable **list, 
                                                     const int length,
                                                     CCObjectCollideable *sourceObject,
                                                     const CCVector3 &start,
                                                     const CCVector3 &end,
                                                     CCVector3 *hitLocation,
                                                     const bool collideInsideObjects,
                                                     const CCCollisionFlags flags=collision_box,
                                                     const bool stopAtAnyCollision = false);

extern CCObjectCollideable* CCBasicLineCollisionCheck(const CCVector3 &start, 
                                                     const CCVector3 &end,
                                                     const float width,
                                                     CCObjectCollideable *source=NULL);

extern bool CCBasicLineCollisionCheck(CCObjectCollideable *checkingObject,
                                      CCObjectCollideable *sourceObject,
                                      const CCVector3 &start,
                                      const CCVector3 &end);

// Frustum testing
extern uint CCSphereInFrustum(float frustum[6][4], const float x, const float y, const float z, const float radius);
extern uint CCCubeInFrustum(const float frustum[6][4], const CCVector3 min, const CCVector3 max);


#endif // __CCCOLLISIONTOOLS_H__
