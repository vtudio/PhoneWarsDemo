/*-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CharacterPlayerAndroid.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"
#include "CCModelObj.h"
#include "CCFileManager.h"


CharacterPlayerAndroid* CharacterPlayerAndroid::Spawn(const char *type)
{
    CharacterPlayerAndroid *character = new CharacterPlayerAndroid();
    character->type = type;
    
    CCText path = "Resources/androids/bots/";
    path += type;
    path += "/";
    path += type;
    path += "_";
    
    {
        CCText objPath = path;
        objPath += "body.obj";
        CCText texPath = path;
        texPath += "diffuse.png";
        
        character->setupBody( objPath.buffer, texPath.buffer );
    }
    
    {
        CCText objPath = path;
        objPath += "screen.obj";
        CCText texPath = path;
        texPath += "screen_diffuse.png";
        
        if( CCFileManager::DoesFileExist( objPath.buffer, Resource_Packaged ) )
        {
            character->setupScreen( objPath.buffer, texPath.buffer );
        }
    }
    
    {
        CCText objPath = path;
        objPath += "head.obj";
        CCText texPath = path;
        texPath += "diffuse.png";
        
        character->setupHead( objPath.buffer, texPath.buffer );
    }

    {
        CCText objPath = path;
        objPath += "leftarm.obj";
        CCText texPath = path;
        texPath += "diffuse.png";
        
        if( CCFileManager::DoesFileExist( objPath.buffer, Resource_Packaged ) )
        {
            character->setupLeftArm( objPath.buffer, texPath.buffer );
        }
    }
    
    {
        CCText objPath = path;
        objPath += "rightarm.obj";
        CCText texPath = path;
        texPath += "diffuse.png";
        
        character->setupRightArm( objPath.buffer, texPath.buffer );
    }
    
    character->finalizeModel();
    character->setupWeapon( character->modelRightArm );
    
    if( character->modelLeftArm != NULL )
    {
        character->setupWeapon2( character->modelLeftArm );
        character->weapon->setFireRate( 0.2f );
        character->weapon2->setFireRate( 0.2f );
    }
    
    return character;
}


CharacterPlayerAndroid::CharacterPlayerAndroid(const float inMovementSpeed) :
    CharacterPlayer( inMovementSpeed )
{	
    modelScreen = NULL;
    modelLeftArm = modelRightArm = NULL;
    weapon2 = NULL;
    weaponSwitchTimeOut = 0.0f;
}


void CharacterPlayerAndroid::setupScreen(const char *modelFile, const char *textureFile)
{
    modelScreen = setupModel( modelFile, textureFile );
}


void CharacterPlayerAndroid::setupLeftArm(const char *modelFile, const char *textureFile)
{
    modelLeftArm = setupModel( modelFile, textureFile );
}


void CharacterPlayerAndroid::setupRightArm(const char *modelFile, const char *textureFile)
{
    modelRightArm = setupModel( modelFile, textureFile );
}


void CharacterPlayerAndroid::finalizeModel()
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
    
    const float collisionSize = 15.0f;
    {
        // Adjust model size
        float scaleFactor = collisionSize / modelWidth;
        if( modelHeight * scaleFactor > 20.0f )
        {
            scaleFactor = 20.0f / modelHeight;
        }
        {
            CCVector3FillPtr( &model->scale, scaleFactor, scaleFactor, scaleFactor );
            
            modelWidth *= scaleFactor;
            modelDepth *= scaleFactor;
            modelHeight *= scaleFactor;
        }
    }
    
    setSquareCollisionBounds( collisionSize * 1.25f, modelHeight );
    setPositionXYZ( 0.0f, collisionBounds.y, 0.0f );
    
    if( modelScreen != NULL )
    {
        modelScreen->setPositionXYZ( 0.0f, modelBody->getPosition().y, -modelBody->getDepth() * 0.5f );
    }
    
    if( modelLeftArm != NULL )
    {
        const float combinedWidth = modelBody->getWidth() * 0.5f + modelLeftArm->getWidth() * 0.5f;
        const float y = ( bodyHeight * 0.25f ) - ( modelLeftArm->getHeight() * 0.5f );
        modelLeftArm->setPositionXYZ( -combinedWidth, y, -modelLeftArm->getDepth() * 0.25f );
    }
    
    if( modelRightArm != NULL )
    {
        const float combinedWidth = modelBody->getWidth() * 0.5f + modelRightArm->getWidth() * 0.5f;
        const float y = ( bodyHeight * 0.25f ) - ( modelRightArm->getHeight() * 0.5f );
        modelRightArm->setPositionXYZ( combinedWidth, y, -modelRightArm->getDepth() * 0.25f );
    }

    setRotationY( 180.0f );
}


void CharacterPlayerAndroid::setupWeapon2(CCModelObj *modelObj)
{
    if( weapon2 != NULL )
    {
        removeChild( weapon2 );
        DELETE_OBJECT( weapon2 );
    }
    
    weapon2 = new WeaponBase( this );
    weapon2->setup( modelObj );
}


// ObjectBase
bool CharacterPlayerAndroid::update(const CCTime &time)
{	
	return super::update( time );
}


void CharacterPlayerAndroid::renderModel(const bool alpha)
{
    super::renderModel( alpha );
}


// CCObjectCollideable
CCObjectCollideable* CharacterPlayerAndroid::requestCollisionWith(CCObjectCollideable *collisionTarget)
{
    return super::requestCollisionWith( collisionTarget );;
}


CCObjectCollideable* CharacterPlayerAndroid::recieveCollisionFrom(CCObjectCollideable *collisionSource, const float x, const float y, const float z)
{
	return super::recieveCollisionFrom( collisionSource, x, y, z );
}


bool CharacterPlayerAndroid::reportAttack(CCObject *attackedBy, const float force, const float damage, const float x, const float y, const float z)
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


void CharacterPlayerAndroid::updateWeaponAim(const float timeDelta)
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
            if( weapon2 != NULL )
            {
                weapon2->updateParentRotation( model->rotation.y );
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
            if( weapon2 != NULL )
            {
                weapon2->updateParentRotation( model->rotation.y );
            }
        }
    }
}


void CharacterPlayerAndroid::shootWeapon(const CCVector3 &target)
{
    scaleShootingIndicator( 5.0f );
    
    if( aimWeapon( target ) )
    {
        if( weapon != NULL && weapon->ready() )
        {
            weapon->shootWeapon( modelRotationTarget );
            
            // Can't switch weapons without a tiny delay
            weaponSwitchTimeOut = gEngine->time.lifetime + 0.05f;
        }
        else if( weapon2 != NULL && weapon2->ready() )
        {
            if( weaponSwitchTimeOut < gEngine->time.lifetime )
            {
                weapon2->shootWeapon( modelRotationTarget );
            }
        }
    }
}