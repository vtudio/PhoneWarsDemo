/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCControllerMovement.h
 * Description : Handles the movement and rotation of an object.
 *
 * Created     : 27/06/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCObjects.h"
#include "CCControllerMovement.h"


CCControllerMovement::CCControllerMovement(CCObjectCollideable *inObject)
{
    object = inObject;

    movementSpeed = 0.0f;

    movementMaxSpeed = 10.0f;
    movementMinSpeed = 1.0f;
    movementAcceleration = 10.0f;
    movementDeceleration = 10.0f;
    movementRotationAngle = 90.0f;
    rotationSpeed = 360.0f;

    findingTarget = false;
}


bool CCControllerMovement::update(const float delta)
{
    if( findingTarget )
    {
        // Target
        {
            const CCVector3 &position = *object->positionPtr;

            // Controls X/Z direction
            const float targetRotationY = CCAngleTowards( position.x, position.z, positionTarget.x, positionTarget.z );
            CCToRotation( movementRotation.y, targetRotationY, rotationSpeed * delta );

            // Controls Y direction
            const float targetRotationX = CCAngleTowards( position.z, position.y, 0.0f, positionTarget.y );
            CCToRotation( movementRotation.x, targetRotationX, rotationSpeed * delta );

            rotationDistance = ( CCDistanceBetweenAngles( movementRotation.y, targetRotationY ) + CCDistanceBetweenAngles( movementRotation.x, targetRotationX ) ) * 0.5f;

            if( rotationDistance <= movementRotationAngle )
            {
                CCToTarget( movementSpeed, movementMaxSpeed, movementAcceleration * delta );
            }
            else
            {
                CCToTarget( movementSpeed, movementMinSpeed, movementDeceleration * delta );
            }
        }

        // Move
        {
            const float movementMagnitude = applyMovementDirection();
            if( movementMagnitude > 0.0f )
            {
                applyVelocity( delta, movementMagnitude );

                object->dirtyModelMatrix();
                object->updateCollisions = true;
                CCOctreeRefreshObject( object );
            }
        }

        return true;
    }
    return false;
}


void CCControllerMovement::setTarget(const CCVector3 &target)
{
    positionTarget = target;
    if( positionTarget != object->getPosition() )
    {
        findingTarget = true;
    }
}


float CCControllerMovement::applyMovementDirection()
{
    if( movementSpeed > 0.0f )
    {
        // Z movement
        float rotationInRadians = CC_DEGREES_TO_RADIANS( movementRotation.y );
        float xAmount = sinf( rotationInRadians ) * -movementSpeed;
        float zAmount = cosf( rotationInRadians ) * -movementSpeed;

        rotationInRadians = CC_DEGREES_TO_RADIANS( movementRotation.x );
        float yAmount = cosf( rotationInRadians ) * -movementSpeed;

        movementVelocity.x = xAmount;
        movementVelocity.y = yAmount;
        movementVelocity.z = zAmount;
    }

    return movementSpeed;
}


void CCControllerMovement::applyVelocity(const float delta, const float movementMagnitude)
{
    CCVector3 &position = *object->positionPtr;

    const float velocityX = movementVelocity.x * delta;
    const float velocityY = movementVelocity.y * delta;
    const float velocityZ = movementVelocity.z * delta;
    if( velocityX != 0.0f || velocityY != 0.0f || velocityZ != 0.0f )
    {
        position.x += velocityX;
        position.y += velocityY;
        position.z += velocityZ;

        const float distance = CCVector3Distance( position, positionTarget );
        if( distance < 1.0f )
        {
            position = positionTarget;
            findingTarget = false;
        }
    }
}
