/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : SceneGameSyndicate.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"
#include "CCModelObj.h"
#include "CCFileManager.h"


SceneGameSyndicate::SceneGameSyndicate(const char *gameID)
{
    cameraCentered = true;
    
    frameBufferID = -1;
    
    setGameID( gameID );
    
    playerCharacter = NULL;
    playerDestinationPending = NULL;
    playerDestinationIndicator = NULL;
    
    oneTouchDoubleTapped = oneTouchDoubleTappedLastPress = false;
    twoTouchAction = twotouch_unassigned;
}


void SceneGameSyndicate::destruct()
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
void SceneGameSyndicate::setup()
{
    // Create a new camera
    camera = new CCCameraAppUI();
    gEngine->addCamera( camera, 0 );
    
    // Set it up to take up the entire screen
    camera->setupViewport( 0.0f, 0.0f, 1.0f, 1.0f, frameBufferID );
    
    camera->targetOffset.z = 120.0f;
    camera->setRotationX( -30.0f );
    
    // Initialize our path finder
	pathFinderNetwork.clear();
    
    createEnvironment();
    
	//pathFinderNetwork.addFillerNodes( ground );

    setupPlayerDestinationIndicator( "Resources/common/fx/fx_character_highlight_texture.png",
                                     CCColour( 0.75f, 0.75f, 1.0f, 1.0f ) );
    
    // refresh the scene range
    refreshCameraView();
    
    sceneGameUI = new SceneGameUI( this );
    gEngine->addScene( sceneGameUI );
}


void SceneGameSyndicate::deleteLater()
{
    super::deleteLater();
}


// CCSceneBase
bool SceneGameSyndicate::updateScene(const CCTime &time)
{    
    // Always fade out our destination indicator
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
    
    // Move the player
    if( playerDestinationPending != NULL )
    {
        const CCScreenTouches *touches = gEngine->controls->getScreenTouches();
        if( touches[0].lastTimeReleased > CC_DOUBLE_TAP_THRESHOLD )
        {
            if( controlsMoving == false )
            {
                playerCharacter->controller->goToScan( *playerDestinationPending );
                DELETE_POINTER( playerDestinationPending );
            }
        }
    }
    
    return super::updateScene( time );
}


bool SceneGameSyndicate::updateCamera(const CCTime &time)
{
    if( playerCharacter != NULL &&
       ( camera->targetLookAt.x != playerCharacter->positionPtr->x || 
        camera->targetLookAt.z != playerCharacter->positionPtr->z ) )
    {
        camera->targetLookAt.x = playerCharacter->positionPtr->x;
        camera->targetLookAt.z = playerCharacter->positionPtr->z;
        camera->flagUpdate();
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


bool SceneGameSyndicate::render(const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    if( super::render( inCamera, pass, alpha ) )
    {
        if( CCHasFlag( gEngine->renderFlags, render_pathFinder ) )
        {
            pathFinderNetwork.view();
        }
    }
    return false;
}


void SceneGameSyndicate::renderOctreeObject(CCObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    // Called on render by our octree which handles drawing objects only in view
    if( camera == inCamera )
    {
        object->renderObject( inCamera, alpha );
    }
}


void SceneGameSyndicate::beginOrientationUpdate()
{
}


void SceneGameSyndicate::finishOrientationUpdate()
{
}


bool SceneGameSyndicate::touchAllowed(const CCScreenTouches &touch)
{
    return playerCharacter != NULL && super::touchAllowed( touch );
}


bool SceneGameSyndicate::handleOneTouch(const CCScreenTouches &touch1)
{
    bool usingControls = false;
    
    // Disable double tap checks
    if( false && oneTouchDoubleTappedLastPress == false )
    {
        if( touch1.lastTimeReleased < CC_DOUBLE_TAP_THRESHOLD )
        {
            if( fabsf( touch1.lastTotalDelta.x ) < CCControls::GetTouchMovementThreashold().x && 
                fabsf( touch1.lastTotalDelta.y ) < CCControls::GetTouchMovementThreashold().y )
            {
                oneTouchDoubleTapped = true;
            }
            
            DELETE_POINTER( playerDestinationPending );
        }
    }
    
    if( oneTouchDoubleTapped )
    {
        // Touch held down after a double tap
        if( touch1.timeHeld >= CC_DOUBLE_TAP_THRESHOLD )
        {
        }
    }
    else
    {
        CCPoint touchDelta = touch1.delta;
        if( controlsMoving == false )
        {
            if( touchMovementAllowed( touch1, touchDelta ) )
            {
            }
            else
            {
                usingControls = touchPressed( touch1 );
            }
        }
        
        if( controlsMoving )
        {   
            usingControls = touchMoving( touch1, touchDelta );
        }
    }
    
    return usingControls;
}


bool SceneGameSyndicate::handleTwoTouches(const CCScreenTouches &touch1, const CCScreenTouches &touch2)
{
    return false;
}


// Callback for when a touch is first detected
void SceneGameSyndicate::touchRegistered(const CCScreenTouches &touch)
{
    twoTouchAction = twotouch_unassigned;
    super::touchRegistered( touch );
}


void SceneGameSyndicate::twoTouchRegistered(const CCScreenTouches &touch1, const CCScreenTouches &touch2)
{
    twoTouchAction = twotouch_unassigned;
    super::twoTouchRegistered( touch1, touch2 );
}


// Callback for when a touch is pressed
bool SceneGameSyndicate::touchPressed(const CCScreenTouches &touch)
{
    return super::touchPressed( touch );
}


bool SceneGameSyndicate::touchMovementAllowed(const CCScreenTouches &touch, CCPoint &touchDelta)
{
    const float absDeltaX = fabsf( touch.totalDelta.x );
    const float absDeltaY = fabsf( touch.totalDelta.y );
    if( absDeltaX > CCControls::GetTouchMovementThreashold().x || absDeltaY > CCControls::GetTouchMovementThreashold().y )
    {
        controlsMoving = true;
        touchDelta.x += touch.totalDelta.x;
        touchDelta.y += touch.totalDelta.y;
        
        controlsMovingVertical = absDeltaY > absDeltaX;
        
        CCVector3 plane;
        camera->project3DY( &plane, touch.position.x, touch.position.y );
        plane.sub( camera->getLookAt() );
        cameraPanningFrom = plane;
        return true;
    }
    return false;
}


bool SceneGameSyndicate::touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta)
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
bool SceneGameSyndicate::touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction)
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
        else if( playerCharacter != NULL )
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
                            playerCharacter->controller->shoot( hitObject );
                        }
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



bool SceneGameSyndicate::touchCameraMoving(const CCScreenTouches &touch, const float x, const float y)
{   
    bool updating = true;
    if( y != 0.0f )
    {
        float delta = y * camera->cameraHeight;
        if( camera->targetLookAt.z > sceneTop || camera->targetLookAt.z < sceneBottom )
        {
            delta *= 0.5f;
        }
        camera->targetLookAt.z -= delta;
        camera->flagUpdate();
        updating = true;
    }
    
    if( x != 0.0f )
    {
        float delta = x * camera->cameraWidth;
        if( camera->targetLookAt.x < sceneLeft || camera->targetLookAt.x > sceneRight )
        {
            delta *= 0.5f;
        }
        camera->targetLookAt.x -= delta;
        camera->flagUpdate();
        updating = true;
    }
    return updating;
}


bool SceneGameSyndicate::touchCameraZooming(const float amount)
{
    return false;
}


bool SceneGameSyndicate::touchCameraRotating(const float x, const float y)
{
    camera->incrementRotationY( -x * 90.0f );
    return true;
}


bool SceneGameSyndicate::touchReleaseSwipe(const CCScreenTouches &touch)
{
    return false;
}


void SceneGameSyndicate::refreshCameraView()
{
    sceneLeft = -mapBounds.width;
    sceneRight = mapBounds.width;
    sceneTop = mapBounds.height;
    sceneBottom = -mapBounds.height;
}


void SceneGameSyndicate::lockCameraView()
{
    if( cameraScrolling )
    {
        return;
    }
    
    if( camera->targetLookAt.x > sceneRight )
    {
        camera->targetLookAt.x = sceneRight;
        camera->flagUpdate();
    }
    else if( camera->targetLookAt.x < sceneLeft )
    {
        camera->targetLookAt.x = sceneLeft;
        camera->flagUpdate();
    }
    
    if( camera->targetLookAt.z > sceneTop )
    {
        camera->targetLookAt.z = sceneTop;
        camera->flagUpdate();
    }
    else if( camera->targetLookAt.z < sceneBottom )
    {
        camera->targetLookAt.z = sceneBottom;
        camera->flagUpdate();
    }
}


void SceneGameSyndicate::setFrameBufferID(const int frameBufferID)
{
    this->frameBufferID = frameBufferID;
    camera->setupViewport( camera->getViewportX(), camera->getViewportY(), camera->getViewportX2(), camera->getViewportY2(), frameBufferID );
    
    if( frameBufferID == -1 )
    {
        if( sceneGameUI == NULL )
        {
            sceneGameUI = new SceneGameUI( this );
            gEngine->addScene( sceneGameUI );
        }
    }
    else
    {
        if( sceneGameUI != NULL )
        {
            sceneGameUI->unlinkScene( this );
            unlinkScene( sceneGameUI );
            sceneGameUI->deleteLater();
            sceneGameUI = NULL;
        }
    }
}


void SceneGameSyndicate::createEnvironment()
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
    
    // Create walls around edges of map
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
    
    // Create sandbags around the level
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


void SceneGameSyndicate::setupPlayerDestinationIndicator(const char *textureFile, CCColour colour)
{
    if( playerDestinationIndicator != NULL )
    {
        playerDestinationIndicator->deleteLater();
    }
    playerDestinationIndicator = new ObjectIndicator( this, textureFile );
    playerDestinationIndicator->setWidth( mapBounds.width * 0.1f );
    playerDestinationIndicator->model->setColour( colour );
}


void SceneGameSyndicate::adjustCollisionPlacement(CCObjectCollideable *source)
{
    if( source->getPosition().y < source->collisionBounds.y )
    {
        source->setPositionY( source->collisionBounds.y );
    }
    
	const uint collisionFlags = collision_static | collision_character;
	CCObjectCollideable *collision = NULL;
	do
	{
		CCUpdateCollisions( source, false );
		collision = CCOctreeCollisionCheck( source->min, source->max, source, collisionFlags );
		if( collision != NULL )
		{
			source->translate( 0.0f, 0.0f, 10.0f );
		}
	} while( collision != NULL );
}


CharacterPlayer* SceneGameSyndicate::spawnCharacter(const char *playerID, const char *type)
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


void SceneGameSyndicate::deletingCharacter(CharacterPlayer *character)
{
    friends.remove( character );
}


PickupBase* SceneGameSyndicate::spawnPickup(const char *name, const char *type, const char *pickupID)
{
    PickupBase *pickup = new PickupBase( name, type, pickupID );
    
    CCText path = "Resources/common/pickups/";
    path += name;
    
    CCText objPath = path;
    objPath += ".obj";
    CCText texPath = path;
    texPath += "_diffuse.png";
    
    pickup->setup( objPath.buffer, 
                   texPath.buffer );
    
    // Don't spawn in place of a building
    adjustCollisionPlacement( pickup );
    
    pickup->setScene( this );
    pickups.add( pickup );
    
    return pickup;
}


void SceneGameSyndicate::assignPlayerCharacter(CharacterPlayer *character)
{   
    ASSERT( playerCharacter == NULL );
    character->setupCharacterIndicator( "Resources/common/fx/fx_character_highlight_texture.png",
                                        CCColour( 0.35f, 0.5f, 0.75f, 1.0f ) );
    
    character->setupShootingIndicator( "Resources/common/fx/fx_character_shooting_texture.png",
                                       CCColour( 0.5f, 0.75f, 1.0f, 0.9f ) );
    
    playerCharacter = character;
}


void SceneGameSyndicate::addFriend(CharacterPlayer *character)
{
    character->setupCharacterIndicator( "Resources/common/fx/fx_character_highlight_texture.png",
                                        CCColour( 0.75f, 0.25f, 0.25f, 1.0f ) );
    
    character->setupShootingIndicator( "Resources/common/fx/fx_character_shooting_texture.png",
                                       CCColour( 1.0f, 0.75f, 0.25f, 0.9f ) );
    
    friends.add( character );
}


void SceneGameSyndicate::removeFriend(const char *playerID)
{
    for( int i=0; i<friends.length; ++i )
    {
        CharacterPlayer *friendCharacter = friends.list[i];
        if( CCText::Equals( playerID, friendCharacter->getPlayerID() ) )
        {
            friendCharacter->deleteLater();
            friends.remove( friendCharacter );
        }
    }
}


CharacterPlayer* SceneGameSyndicate::getPlayer(const char *playerID)
{
    if( playerCharacter != NULL )
    {
        if( CCText::Equals( playerCharacter->getPlayerID(), playerID ) )
        {
            return playerCharacter;
        }
    }
    
    for( int i=0; i<friends.length; ++i )
    {
        CharacterPlayer *friendCharacter = friends.list[i];
        if( CCText::Equals( friendCharacter->getPlayerID(), playerID ) )
        {
            return friendCharacter;
        }
    }
    
    return NULL;
}


CharacterPlayer* SceneGameSyndicate::getFriend(CCObject *object)
{
    for( int i=0; i<friends.length; ++i )
    {
        CharacterPlayer *friendCharacter = friends.list[i];
        if( friendCharacter == object )
        {
            return friendCharacter;
        }
    }
    
    return NULL;
}


void SceneGameSyndicate::registerAttack(CCObject *from, CCObject *to, const float force)
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


void SceneGameSyndicate::handlePickup(CharacterPlayer *player, PickupBase *pickup)
{
    player->controller->resetHealth();
    const float healthRatio = player->controller->getHealthRatio();
    if( player == playerCharacter )
    {
        sceneGameUI->setHealthAlpha( 0, healthRatio );
    }
    else
    {
        sceneGameUI->setHealthAlpha( 1, healthRatio );
    }
    pickup->deleteLater();
    pickups.remove( pickup );
}


void SceneGameSyndicate::setLocation(const char *playerID, const CCVector3 &location)
{
    if( playerCharacter != NULL && CCText::Equals( playerCharacter->getPlayerID(), playerID ) )
    {
        playerCharacter->setPosition( location );
        adjustCollisionPlacement( playerCharacter );
    }
    else
    {
        CharacterPlayer *player = getPlayer( playerID );
        if( player != NULL )
        {
            player->setPosition( location );
            adjustCollisionPlacement( player );
        }
    }
}


void SceneGameSyndicate::pause()
{
    enabled = false;
    DELETE_POINTER( playerDestinationPending );
}


void SceneGameSyndicate::unPause()
{
    enabled = true;   
}


const int SceneGameSyndicate::getNumberOfPlayers()
{
    int count = 0;
    if( playerCharacter != NULL )
    {
        count++;
    }
    count += friends.length;
    return count;
}
