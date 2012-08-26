/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : WeaponBase.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"
#include "CCModelObj.h"
#include "CCFileManager.h"


WeaponBase::WeaponBase(CharacterPlayer *character)
{
    this->character = character;
    
    fireRate = 0.1f;
    shootingTimer = 0.0f;
    shootingTimeSinceLastShot = 0.0f;
}


void WeaponBase::setup(CCModelObj *modelObj)
{
    model = new CCModelBase();
    model->setColour( CCColour() );
    model->setPosition( character->model->getPosition() );
    
    modelWeapon = modelObj;
    modelWeaponHomePosition = modelWeapon->getPosition();
    modelWeaponMovementInterpolator.setup( modelWeapon->positionPtr, modelWeaponHomePosition );
    modelWeaponMovementInterpolator.setDuration( 0.05125f );
    
    // Muzzle flash
    objectMuzzleFlash = NULL;
    modelMuzzleFlash = NULL;
    {
        CCText path = "Resources/common/fx/fx_weapon_muzzleflash";
        
        CCText objFile = path.buffer;
        objFile += ".obj";
        
        CCText texFile = path.buffer;
        texFile += "_texture.png";  
        
        objectMuzzleFlash = new CCObject();
        objectMuzzleFlash->model = new CCModelBase();
        objectMuzzleFlash->model->setPosition( model->getPosition() );
        objectMuzzleFlash->model->setColour( CCColour() );
        
        objectMuzzleFlash->setTransparent();
        objectMuzzleFlash->readDepth = true;
        objectMuzzleFlash->writeDepth = false;
        objectMuzzleFlash->disableCulling = true;
        addChild( objectMuzzleFlash );
        
        float x = modelObj->getPosition().x;
        float y = modelObj->getPosition().y;
        float z = modelObj->getPosition().z;
        float weaponDepth = modelObj->getDepth();
        
        // Realign weapon based on end point
        {
            //const CCMinMax weaponHeightMinMax = modelObj->getPrimitive()->getYMinMax();
            const CCMinMax weaponDepthMinMax = modelObj->getPrimitive()->getZMinMax();
            const CCMinMax weaponHeightMinMaxAtPoint = modelObj->getPrimitive()->getYMinMaxAtZ( weaponDepthMinMax.max * 0.9f );
            
            const float centerPointY = weaponHeightMinMaxAtPoint.min + weaponHeightMinMaxAtPoint.size() * 0.5f;
            y += centerPointY;
        }
        
        CCVector3 *scale = character->model->scale;
        if( scale != NULL )
        {
            x *= scale->x;
            y *= scale->y;
            z *= scale->z;
            weaponDepth*= scale->z;
        }
        z -= weaponDepth * 0.5f;
        
        spawnPoint = CCVector3( x, y, z );
        
        modelMuzzleFlash = CCModelObj::CacheModel( objFile.buffer, texFile.buffer );
        modelMuzzleFlash->setPosition( spawnPoint );
        modelMuzzleFlash->translate( 0.0f, 0.0f, -modelMuzzleFlash->getDepth() * 0.5f );
        objectMuzzleFlash->model->addModel( modelMuzzleFlash );
        
        objectMuzzleFlash->shouldRender = true;
    }
    
    character->addChild( this );
}


void WeaponBase::destruct()
{
    super::destruct();
}


const bool WeaponBase::ready()
{
    return shootingTimeSinceLastShot > fireRate;
}


void WeaponBase::shootWeapon(const float characterRotation)
{
    if( modelWeaponMovementInterpolator.isUpdating() == false )
    {
        LAMBDA_1( AnimCallback, WeaponBase, weapon,
                 {
                     weapon->modelWeaponMovementInterpolator.setup( weapon->modelWeapon->positionPtr, weapon->modelWeaponHomePosition );
                 });
        modelWeaponMovementInterpolator.setup( modelWeapon->positionPtr, 
                                               CCVector3( modelWeaponHomePosition.x, modelWeaponHomePosition.y, modelWeaponHomePosition.z+1.0f ),
                                               new AnimCallback( this ) );
    }
    
    ObjectMoveableBullet *bullet = new ObjectMoveableBullet( 10.0f );
    bullet->setSquareCollisionBounds( 5.0f );
    bullet->setTransparent();
    bullet->readDepth = true;
    
    CCText path = "Resources/common/fx/fx_weapon_bullet";
    {
        CCText objPath = path;
        objPath += ".obj";
        
        CCText texPath = path;
        texPath += "_texture.png";
        
        CCModelObj *model3d = CCModelObj::CacheModel( objPath.buffer, texPath.buffer );
        
        bullet->model = model3d;
        bullet->movementDirection.z = -1.0f;
        
    }
    
    ASSERT( character->inScene != NULL );
    bullet->setScene( character->inScene );
    character->ownObject( bullet );
    
    {
        CCText texPath = path;
        texPath += "_glow_texture.png";
        
        ObjectIndicator *objectIndicator = new ObjectIndicator( bullet, texPath.buffer );
        CCVector3FillPtr( &objectIndicator->scale, 5.0f );
        objectIndicator->translate( 0.0f, -character->collisionBounds.y, 0.0f );
    }
        
    setupSpawnPoint( bullet, characterRotation );
    shoot();
    shootingTimeSinceLastShot = 0.0f;
}


void WeaponBase::setupSpawnPoint(CCObjectCollideable *projectile, const float characterRotation)
{   
    CCPoint spawnPointXZ = CCPoint( this->spawnPoint.x, this->spawnPoint.z );
    CCRotatePoint( spawnPointXZ, characterRotation );
    
    CCVector3 spawnPoint = parent->getPosition();
    spawnPoint.x += spawnPointXZ.x;
    spawnPoint.y += this->spawnPoint.y;
    spawnPoint.z += spawnPointXZ.y;
    
    projectile->setPosition( spawnPoint );
    projectile->rotation.y = characterRotation;
}


void WeaponBase::updateParentRotation(const float rotation)
{
    model->rotation.y = rotation;
    model->dirtyModelMatrix();
    
    if( objectMuzzleFlash != NULL )
    {
        objectMuzzleFlash->rotation.y = rotation;
        objectMuzzleFlash->dirtyModelMatrix();
    }
}


bool WeaponBase::update(const CCTime &time)
{
    bool updated = super::update( time );
    
    if( modelWeaponMovementInterpolator.update( time.delta ) )
    {
        modelWeapon->dirtyModelMatrix();
        updated = true;
    }
    
    if( objectMuzzleFlash != NULL )
    {
        if( objectMuzzleFlash->shouldRender )
        {
            if( CCToTarget( shootingTimer, 0.0f, time.delta ) )
            {
                if( objectMuzzleFlash != NULL )
                {
                    modelMuzzleFlash->rotateZ( time.delta * 360.0f );
                }
            }
            else
            {
                objectMuzzleFlash->shouldRender = false;
            }
        }
    }
    
    shootingTimeSinceLastShot += time.delta;
    
    return updated;
}


void WeaponBase::renderObject(const CCCameraBase *camera, const bool alpha)
{
    super::renderObject( camera, alpha );
}


void WeaponBase::renderModel(const bool alpha)
{
    super::renderModel( alpha );
}


void WeaponBase::shoot()
{
    shootingTimer = 0.025f;
    objectMuzzleFlash->shouldRender = true;
}
