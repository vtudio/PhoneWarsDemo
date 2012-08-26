/*-----------------------------------------------------------
 * http://softpoetry.com
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CharacterPlayer.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"
#include "CCModelObj.h"
#include "CCFileManager.h"


CharacterPlayer::CharacterPlayer(const float inMovementSpeed)
{	
    movementSpeed = inMovementSpeed;
    
	CCAddFlag( collideableType, collision_character );
	
    controller = NULL;
	gravity = false;
	
    modelBody = modelHead = NULL;
    weapon = NULL;
    
    model = new CCModelBase();
    model->setColour( CCColour() );
    
    modelRotationTargetActive = false;
    modelRotationTarget = 0.0f;
    
    modelHeadAnimationActive = false;
    modelHeadAnimationState = 0;
    
    characterIndicator = NULL;
    shootingIndicator = NULL;
    shootingScaleInterpolator.setDuration( 0.5f );
    
    lastTimeAttacked = 0.0f;
	deathFadeOut = false;
}


void CharacterPlayer::setupGame(SceneGameSyndicate *sceneGame, const char *playerID)
{    
    setScene( sceneGame );
    this->sceneGame = sceneGame;
    this->playerID = playerID;
}


void CharacterPlayer::setupAI(CharacterUpdaterPlayer *controller)
{
    if( this->controller != NULL )
    {
        removeUpdater( this->controller );
        DELETE_OBJECT( this->controller );
    }
	this->controller = controller;
	controller->setPlayer( this );
	addUpdater( controller );
}


void CharacterPlayer::setupBody(const char *modelFile, const char *textureFile)
{
    modelBody = setupModel( modelFile, textureFile );
}


void CharacterPlayer::setupHead(const char *modelFile, const char *textureFile)
{
    modelHead = setupModel( modelFile, textureFile );
}


void CharacterPlayer::finalizeModel()
{
    float modelWidth = modelBody->getWidth();
    float modelDepth = modelBody->getDepth();
    const float bodyHeight = modelBody->getHeight();
    float modelHeight = bodyHeight;
    if( modelHead != NULL )
    {
        modelHeight += modelHead->getHeight();
        modelBody->setPositionXYZ( 0.0f, -modelHead->getHeight() * 0.5f, 0.0f );
        modelHead->setPositionXYZ( 0.0f, modelBody->getHeight() * 0.5f, -modelBody->getDepth() * 0.5f + modelHead->getDepth() * 0.5f );
    }
    
    {
        // Adjust model height
        const float scaleFactor = 15.0f / modelHeight;
        CCVector3FillPtr( &model->scale, scaleFactor, scaleFactor, scaleFactor );
        
        modelWidth *= scaleFactor;
        modelDepth *= scaleFactor;
        modelHeight *= scaleFactor;
    }
    
    setSquareCollisionBounds( modelWidth, modelHeight );
    setPositionXYZ( 0.0f, collisionBounds.y, 0.0f );
    
    setRotationY( 180.0f );
}


void CharacterPlayer::setupWeapon(CCModelObj *modelObj)
{
    if( weapon != NULL )
    {
        removeChild( weapon );
        DELETE_OBJECT( weapon );
    }
    
    weapon = new WeaponBase( this );
    weapon->setup( modelObj );
}


const bool CharacterPlayer::useableWeapon(const char *weaponName)
{
    CCText path = "Resources/characters/";
    path += type.buffer;
    path += "/character_";
    path += type.buffer;
    CCText pathWeapon = path;
    pathWeapon += "_weapon_";
    pathWeapon += weaponName;
    
    CCText modelFile = pathWeapon.buffer;
    modelFile += ".obj";
    
    return CCFileManager::DoesFileExist( modelFile.buffer, Resource_Packaged );
}


CCModelObj* CharacterPlayer::setupModel(const char *modelFile, const char *textureFile)
{
    // Create the model
    CCModelObj *model3d = CCModelObj::CacheModel( modelFile, textureFile );
    //model3d->shader = "phong";
    model->addModel( model3d );
    
    return model3d;
}


void CharacterPlayer::setupCharacterIndicator(const char *textureFile, CCColour colour)
{
    characterIndicator = new ObjectIndicator( this, textureFile );
    characterIndicator->translate( 0.0f, -collisionBounds.y, 0.0f );
    
    CCVector3FillPtr( &characterIndicator->scale, 2.0f );
    characterIndicator->model->setColour( colour );
}


void CharacterPlayer::setupShootingIndicator(const char *textureFile, CCColour colour)
{
    shootingIndicator = new ObjectIndicator( this, textureFile );
    shootingIndicator->translate( 0.0f, -collisionBounds.y, 0.0f );
    
    CCVector3FillPtr( &shootingIndicator->scale, 0.0f );
    shootingIndicator->model->setColour( colour );
}


void CharacterPlayer::scaleShootingIndicator(const float target)
{
    if( shootingIndicator != NULL )
    {
        if( shootingIndicator->scale != NULL )
        {
            shootingScaleInterpolator.setup( shootingIndicator->scale, target );
        }
    }
}


// ObjectBase
bool CharacterPlayer::update(const CCTime &time)
{
	if( deathFadeOut )
	{
		deleteLater();
	}
    
    updateWeaponAim( time.delta );
    
    if( modelHeadAnimationActive && modelHead != NULL )
    {
        const float angle = 5.0f;
        const float speed = time.delta * 30.0f;
        if( modelHeadAnimationState == 0 )
        {
            if( CCToRotation( modelHead->rotation.z, angle, speed ) )
            {
                modelHead->rotationUpdated();
            }
            else
            {
                modelHeadAnimationState = 1;
            }
        }
        else if( modelHeadAnimationState == 1 )
        {
            if( CCToRotation( modelHead->rotation.z, ( 360 - angle ), speed ) )
            {
                modelHead->rotationUpdated();
            }
            else
            {
                modelHeadAnimationState = 0;
            }
        }
    }
    
    if( characterIndicator != NULL )
    {
        characterIndicator->rotateY( time.delta * -180.0f );
    }
    
    if( shootingIndicator != NULL )
    {
        shootingScaleInterpolator.update( time.delta );
        if( shootingIndicator->scale->x > 0.0f )
        {
            shootingIndicator->rotateY( time.delta * 720.0f );
        }
    }
	
	return super::update( time );
}


void CharacterPlayer::renderModel(const bool alpha)
{
    super::renderModel( alpha );
}


// CCObjectCollideable
CCObjectCollideable* CharacterPlayer::requestCollisionWith(CCObjectCollideable *collisionTarget)
{
    CCObjectCollideable *collidedWith = super::requestCollisionWith( collisionTarget );
    if( collidedWith != NULL )
    {   
        if( CCText::Equals( collidedWith->getType(), "health" ) )
        {
            PickupBase *pickup = (PickupBase*)( collidedWith );
            if( pickup != NULL )
            {
                sceneGame->handlePickup( this, pickup );
                collidedWith = NULL;
            }        
        }
        
        if( collidedWith != NULL )
        {
            if( controller != NULL )
            {
                controller->recieveCollisionFrom( collisionTarget );
            }
        }
        
        return collidedWith;
    }
    return NULL;
}


CCObjectCollideable* CharacterPlayer::recieveCollisionFrom(CCObjectCollideable *collisionSource, const float x, const float y, const float z)
{
	if( controller != NULL )
	{
		controller->recieveCollisionFrom( collisionSource );
	}
	return super::recieveCollisionFrom( collisionSource, x, y, z );
}


bool CharacterPlayer::reportAttack(CCObject *attackedBy, const float force, const float damage, const float x, const float y, const float z)
{
	if( isActive() && controller != NULL )
	{
        const float sinceLastAttack = gEngine->time.lifetime - lastTimeAttacked;
        if( sinceLastAttack > 0.25f )
        {
            lastTimeAttacked = gEngine->time.lifetime;
            // Spawn hit particle
            {
                ObjectCollideableParticle *particle = new ObjectCollideableParticle( 3.0f );
                
                particle->model = new CCModelBase();
                particle->model->setColour( CCColour() );
                CCPrimitiveSquare *primitive = new CCPrimitiveSquare();
                primitive->setupZFacing( 1.0f, 1.0f, 0.0f );
                particle->model->addPrimitive( primitive );
                CCVector3FillPtr( &particle->model->scale, 30.0f );
                
                primitive->setTexture( "Resources/common/fx/fx_impact_texture.png", Resource_Packaged, true, true );
                
                ASSERT( inScene != NULL );
                particle->setScene( inScene );
                ownObject( particle );
                
                // Position with the character
                particle->setPosition( position );
                
                // Rotate with the camera
                particle->setRotationY( sceneGame->getCamera()->getRotation() );
            }
        }
        
		return controller->reportAttack( attackedBy, force, damage, x, y, z );
	}
	return false;
}


const bool CharacterPlayer::aimWeapon(const CCVector3 &target)
{
    modelRotationTargetActive = true;
	modelRotationTarget = CCAngleTowards( position, target );
    
    // Is target in view?
    {
        const float baseRotation = 0.0f - rotation.y;
        const float combinedRotation = baseRotation + modelRotationTarget;
        float targetRotation = combinedRotation;
        CCClampRotation( targetRotation );
        const float distance = CCDistanceBetweenAngles( model->rotation.y, targetRotation );
        if( distance < 10.0f )
        {
            return true;
        }
        return false;
    }
}


void CharacterPlayer::updateWeaponAim(const float timeDelta)
{
    if( modelRotationTargetActive )
    {
        const float baseRotation = 0.0f - rotation.y;
        const float combinedRotation = baseRotation + modelRotationTarget;
        float targetRotation = combinedRotation;
        CCClampRotation( targetRotation );
        if( CCToRotation( model->rotation.y, targetRotation, timeDelta * 720.0f ) )
        {
            model->dirtyModelMatrix();
            if( weapon != NULL )
            {
                weapon->updateParentRotation( model->rotation.y );
            }
        }
    }
    else
    {
        if( CCToRotation( model->rotation.y, 0.0f, timeDelta * 360.0f ) )
        {
            model->dirtyModelMatrix();
            
            if( weapon != NULL )
            {
                weapon->updateParentRotation( model->rotation.y );
            }
        }
    }
}


void CharacterPlayer::shootWeapon(const CCVector3 &target)
{
    scaleShootingIndicator( 5.0f );
    
    if( weapon != NULL && weapon->ready() )
    {
        if( aimWeapon( target ) )
        {
            weapon->shootWeapon( modelRotationTarget );
        }
    }
}


void CharacterPlayer::unAimWeapon()
{
    scaleShootingIndicator( 0.0f );
    
    modelRotationTargetActive = false;
    modelRotationTarget = 0.0f;
}


void CharacterPlayer::triggerDeath(CCObject *attackedBy, const float force, const float x, const float y, const float z)
{
	CCRemoveFlag( collideableType, collision_character );
	collideableType = collision_none;
	movementDirection.zero();
	
    removeUpdater( controller );
	DELETE_OBJECT( controller );
    
    if( sceneGame != NULL )
    {
        sceneGame->deletingCharacter( this );
    }
    deleteLater();
}


void CharacterPlayer::animatedDeath()
{
	deathFadeOut = true;
	
	if( owner != NULL )
	{
		owner->unOwnObject( this );
		owner = NULL;
	}
}
