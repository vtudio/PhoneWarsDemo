/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : SceneGameBurgersDeathmatch.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"
#include "CCModelObj.h"
#include "CCFileManager.h"


SceneGameBurgersDeathmatch::SceneGameBurgersDeathmatch(const char *gameID) :
    SceneGameSyndicate( gameID )
{
}


void SceneGameBurgersDeathmatch::destruct()
{
    if( playerCharacter != NULL )
    {
        playerCharacter->deleteLater();
        playerCharacter = NULL;
        DELETE_POINTER( playerDestinationPending );
    }
    
    super::destruct();
}


// Called after our constructor is called
void SceneGameBurgersDeathmatch::setup()
{
    super::setup();
}


void SceneGameBurgersDeathmatch::deleteLater()
{
    super::deleteLater();
}


// CCSceneBase
bool SceneGameBurgersDeathmatch::updateScene(const CCTime &time)
{
    if( playerDestinationIndicator != NULL )
    {
        float alpha = playerDestinationIndicator->model->getColour()->alpha;
        if( alpha > 0.0f )
        {
            playerDestinationIndicator->rotateY( time.delta * 720.0f );
            CCToTarget( alpha, 0.0f, time.delta * 0.5f );
            playerDestinationIndicator->model->setColourAlpha( alpha );
        }
    }
    
    return super::updateScene( time );
}


bool SceneGameBurgersDeathmatch::updateCamera(const CCTime &time)
{
    if( playerCharacter != NULL )
    {
        if( camera->targetLookAt.x != playerCharacter->positionPtr->x ||
            camera->targetLookAt.z != playerCharacter->positionPtr->z )
        {
            camera->targetLookAt.x = playerCharacter->positionPtr->x;
            camera->targetLookAt.z = playerCharacter->positionPtr->z;
            camera->flagUpdate();
        }
    }
    
    if( camera->targetOffset.z != 120.0f )
    {
        camera->targetOffset.z = 120.0f;
    }
    
    bool updated = false;
    
    const float lookAtSpeed = 1.5f;
	if( camera->interpolateCamera( time.delta, lookAtSpeed ) )
    {
        updated = true;
	}
    else
    {
        if( cameraScrolling )
        {
            cameraScrolling = false;
            lockCameraView();
            updated = true;
        }
        
        if( updatingOrientation )
        {
            updatingOrientation = false;
            refreshCameraView();
            lockCameraView();
            updated = true;
        }
    }
    
    return updated;
}


bool SceneGameBurgersDeathmatch::touchAllowed(const CCScreenTouches &touch)
{
    return super::touchAllowed( touch );
}


bool SceneGameBurgersDeathmatch::handleOneTouch(const CCScreenTouches &touch1)
{
    return super::handleOneTouch( touch1 );
}


bool SceneGameBurgersDeathmatch::handleTwoTouches(const CCScreenTouches &touch1, const CCScreenTouches &touch2)
{   
    return false;
}


// Callback for when a touch is first detected
void SceneGameBurgersDeathmatch::touchRegistered(const CCScreenTouches &touch)
{
    super::touchRegistered( touch );
}

void SceneGameBurgersDeathmatch::twoTouchRegistered(const CCScreenTouches &touch1, const CCScreenTouches &touch2)
{
    super::twoTouchRegistered( touch1, touch2 );
}


// Callback for when a touch is pressed
bool SceneGameBurgersDeathmatch::touchPressed(const CCScreenTouches &touch)
{
    return super::touchPressed( touch );
}


bool SceneGameBurgersDeathmatch::touchMovementAllowed(const CCScreenTouches &touch, CCPoint &touchDelta)
{
    return super::touchMovementAllowed( touch, touchDelta );
}


bool SceneGameBurgersDeathmatch::touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta)
{
    // Run through all the tiles
    CCTileTouchResult result = handleTilesTouch( touch, controlsMovingVertical ? touch_movingVertical : touch_movingHorizontal );
    if( result == tile_touchAction )
    {
        return true;
    }
    else
    {
        touchCameraRotating( touch.delta.x, touch.delta.y );
    }
    
    return false;
}


// Callback for when a touch is released
bool SceneGameBurgersDeathmatch::touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction)
{
    // Find pressed tile
    CCTileTouchResult result = handleTilesTouch( touch, touchAction );
    bool usingControls = result == tile_touchAction;
    if( usingControls == false && touchAction == touch_released )
    {
        // Shoot
        if( oneTouchDoubleTapped )
        {
        }
        
        // Panning
        else if( controlsMoving )
        {
            usingControls = touchReleaseSwipe( touch );
        }
        
        // Move player or shoot instead
        else
        {
            // Test if we're aiming at another player
            CCObjectCollideable *hitObject = NULL;
            {
                if( camera->project3D( touch.position.x, touch.position.y ) )
                {
                    const CCCameraProjectionResults &projectionResults = camera->getProjectionResults();
                    
                    // Scan to see if we're blocked by a collision
                    static CCVector3 hitPosition;
                    hitObject = CCBasicLineCollisionCheck( (CCObjectCollideable**)collideables.list,
                                                           collideables.length,
                                                           playerCharacter,
                                                           projectionResults.vNear, projectionResults.vFar,
                                                           &hitPosition,
                                                           true,
                                                           collision_character,
                                                           false );
                    
                    // Fill in the hitPosition variable if nothing has been hit
                    if( hitObject != NULL )
                    {
                        if( hitObject == playerCharacter )
                        {
                            hitObject = NULL;
                        }
                        
                        // Shoot at that player
                        else
                        {
                            playerCharacter->controller->shoot( hitObject );
                        }
                    }
                    
                    // See if there's a target character closeby
                    else
                    {
                        // Project touch to y = 0
                        CCVector3 plane;
                        const float offset = projectionResults.vNear.y / fabsf( projectionResults.vDirection.y );
                        plane = CCVector3MulResult( projectionResults.vDirection, offset );
                        plane.add( projectionResults.vNear );
                        
                        hitObject = playerCharacter->controller->scanForEnemy( plane );
                        if( hitObject != NULL )
                        {
                            playerCharacter->controller->shoot( hitObject );                        }
                    }
                }
            }
            
            if( hitObject == NULL )
            {
                playerDestinationPending = new CCVector3();
                camera->project3DY( playerDestinationPending, touch.position.x, touch.position.y );
                CCClampFloat( playerDestinationPending->x, -mapBounds.width, mapBounds.width );
                CCClampFloat( playerDestinationPending->z, -mapBounds.height, mapBounds.height );

                if( playerDestinationIndicator != NULL )
                {
                    playerDestinationIndicator->setPositionXZ( playerDestinationPending->x, playerDestinationPending->z );
                    playerDestinationIndicator->model->setColourAlpha( 1.0f );
                }
            }
        }
    }
    
    twoTouchAction = twotouch_unassigned;
    
    if( oneTouchDoubleTappedLastPress )
    {
        oneTouchDoubleTappedLastPress = false;
    }
    
    if( oneTouchDoubleTapped )
    {
        oneTouchDoubleTappedLastPress = true;
        oneTouchDoubleTapped = false;
    }
    
    return usingControls;
}



bool SceneGameBurgersDeathmatch::touchCameraMoving(const CCScreenTouches &touch, const float x, const float y)
{
    return super::touchCameraMoving( touch, x, y );
}


bool SceneGameBurgersDeathmatch::touchCameraZooming(const float amount)
{
    return false;
}


bool SceneGameBurgersDeathmatch::touchCameraRotating(const float x, const float y)
{
    camera->incrementRotationY( -x * 90.0f );
    return true;
}


bool SceneGameBurgersDeathmatch::touchReleaseSwipe(const CCScreenTouches &touch)
{
    return false;
}


void SceneGameBurgersDeathmatch::createEnvironment()
{
    {
        CCText levelsPath = "Resources/common/levels/level_";
        
        // Ground
        {
            const float size = 500.0f;
            mapBounds.width = size * 0.5f * 0.8f;
            mapBounds.height = size * 0.5f * 0.8f;
            
            ground = new CollideableFloor();
            ground->setup( size, size );
            ground->setScene( this );
            ground->readDepth = false;
            
            CCText texPath = "Resources/";
            texPath += CLIENT_NAME;
            texPath += "/levels/level_background.png";
            ground->primitive->setTexture( texPath.buffer, Resource_Packaged );
        }
        
        // Particles
        {
            CCText objFile = levelsPath;
            objFile += "particles.obj";
            
            CCText texFile = levelsPath;
            texFile += "particles_diffuse.png";

            {                
                if( CCFileManager::DoesFileExist( objFile.buffer, Resource_Packaged ) )
                {
                    CCObject *object = new CCObject();
                    object->setScene( this );
                    CCModelBase *model = new CCModelBase();
                    CCModelObj *model3d = CCModelObj::CacheModel( objFile.buffer, texFile.buffer, false );
                    model3d->setColour( CCColour( 1.0f ) );
                    model->addModel( model3d );
                    object->model = model;
                    
                    object->setTransparent();
                }
            }
            {   
                if( CCFileManager::DoesFileExist( objFile.buffer, Resource_Packaged ) )
                {
                    CCObject *object = new CCObject();
                    object->setScene( this );
                    CCModelBase *model = new CCModelBase();
                    CCModelObj *model3d = CCModelObj::CacheModel( objFile.buffer, texFile.buffer, false );
                    model3d->setColour( CCColour( 1.0f ) );
                    model->addModel( model3d );
                    object->model = model;
                    
                    object->setTransparent();
                    object->rotateY( 90.0f );
                }
            }
        }
    }
    
    // Walls
	{
		CollideableWall *wall;
		
        const float x = mapBounds.width;
        const float z = mapBounds.height;
		const float width = mapBounds.width * 2.0f;
		const float depth = mapBounds.height * 2.0f;
		const float height = 20.0f;
		
		// Left
		wall = new CollideableWall();
		wall->setup( this, -x, 0.0f, depth, height );
		wall->translate( -wall->collisionBounds.x, 0.0f, 0.0f );
		
		// Right
		wall = new CollideableWall();
		wall->setup( this, x, 0.0f, depth, height );
		wall->translate( wall->collisionBounds.x, 0.0f, 0.0f );
		
		// Back
		wall = new CollideableWall();
		wall->setup( this, 0.0f, -z, width, height );
		wall->translate( 0.0f, 0.0f, -wall->collisionBounds.z );
		
		// Front
		wall = new CollideableWall();
		wall->setup( this, 0.0f, z, width, height );
		wall->translate( 0.0f, 0.0f, wall->collisionBounds.z );
	}
    
    {
        CCList<CCPoint> sandbagLocations;
        sandbagLocations.add( new CCPoint( 0.0f, 0.0f ) );
        sandbagLocations.add( new CCPoint( -50.0f, -100.0f ) );
        sandbagLocations.add( new CCPoint( 50.0f, -100.0f ) );
        sandbagLocations.add( new CCPoint( -50.0f, 100.0f ) );
        sandbagLocations.add( new CCPoint( 50.0f, 100.0f ) );
        
        const float sandbagWidth = 30.0f;
        for( int i=0; i<sandbagLocations.length; ++i )
        {   
            CCText fxPath = "Resources/common/levels/";
            CCText objFile = fxPath;
            objFile += "sandbags.obj";
            
            CCText texFile = fxPath;
            texFile += "sandbags_diffuse.png";
            
            CCObjectCollideable *object = new CCObjectCollideable();
            object->setScene( this );
            CCAddFlag( object->collideableType, collision_static );
            
            CCModelBase *model = new CCModelBase();
            CCModelObj *model3d = CCModelObj::CacheModel( objFile.buffer, texFile.buffer );
            model3d->setColour( CCColour( 1.0f ) );
            model->addModel( model3d );
            object->model = model;
            
            float modelWidth = model3d->getWidth();
            float modelHeight = model3d->getHeight();
            float modelDepth = model3d->getDepth();
            
            // Adjust model height
            const float scaleFactor = sandbagWidth / modelWidth;
            CCVector3FillPtr( &model->scale, scaleFactor, scaleFactor, scaleFactor );
            model->rotateY( 90.0f );
            
            modelWidth *= scaleFactor;
            modelDepth *= scaleFactor;
            modelHeight *= scaleFactor;
            
            object->setCollisionBounds( modelDepth, modelHeight, modelWidth );
            object->translate( 0.0f, object->collisionBounds.y, 0.0f );
            object->setPositionXZ( sandbagLocations.list[i]->x, sandbagLocations.list[i]->y );
            
            pathFinderNetwork.addCollideable( object, ground->collisionBounds );
            
            object->setTransparent();
            object->disableCulling = true;
            object->readDepth = true;
            object->drawOrder = 99;
        }
        
        sandbagLocations.deleteObjects();
    }
}


CharacterPlayer* SceneGameBurgersDeathmatch::spawnCharacter(const char *playerID, const char *type)
{
    CharacterPlayer *character =  ScenePlayManager::SpawnCharacter( type );
    character->setupGame( this, playerID );
    
    // Don't spawn in a bad location
    adjustCollisionPlacement( character );
    
	pathFinderNetwork.connect( character );
    character->setupAI( new CharacterUpdaterPlayer(  &pathFinderNetwork ) );
    
#ifdef DEBUGON
    CCText name = "SceneGameBurgersDeathmatch ";
    name += type;
    character->setDebugName( name.buffer );
#endif
    
    return character;
}


PickupBase* SceneGameBurgersDeathmatch::spawnPickup(const char *name, const char *type, const char *pickupID)
{
    return super::spawnPickup( name, type, pickupID );
}


void SceneGameBurgersDeathmatch::assignPlayerCharacter(CharacterPlayer *character)
{   
    ASSERT( playerCharacter == NULL );
    character->setupCharacterIndicator( "Resources/common/fx/fx_character_highlight_texture.png",
                                        CCColour( 0.35f, 0.5f, 0.75f, 1.0f ) );
    
    character->setupShootingIndicator( "Resources/common/fx/fx_character_shooting_texture.png",
                                       CCColour( 0.5f, 0.75f, 1.0f, 0.9f ) );
    
    playerCharacter = character;
}


void SceneGameBurgersDeathmatch::addFriend(CharacterPlayer *character)
{
    character->setupCharacterIndicator( "Resources/common/fx/fx_character_highlight_texture.png",
                                        CCColour( 0.75f, 0.25f, 0.25f, 1.0f ) );
    
    character->setupShootingIndicator( "Resources/common/fx/fx_character_shooting_texture.png",
                                       CCColour( 1.0f, 0.75f, 0.25f, 0.9f ) );
    
    friends.add( character );
}


void SceneGameBurgersDeathmatch::registerAttack(CCObject *from, CCObject *to, const float force)
{
    CharacterPlayer *friendCharacter = getFriend( to );
    if( friendCharacter != NULL )
    {
        const float healthRatio = friendCharacter->controller->getHealthRatio();
        sceneGameUI->setHealthAlpha( 1, healthRatio );
        if( healthRatio <= 0.0f )
        {
            if( ScenePlayManager::scene != NULL )
            {
                ScenePlayManager::scene->matchEnd();
            }
        }
    }
}
