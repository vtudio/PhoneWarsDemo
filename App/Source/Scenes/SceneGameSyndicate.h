/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : SceneGameSyndicate.h
 * Description : A scene inspired by the Syndicate game.
 *
 * Created     : 17/02/12
 *-----------------------------------------------------------
 */

class SceneGameSyndicate : public CCSceneAppUI
{
public:
    typedef CCSceneAppUI super;

    SceneGameSyndicate(const char *gameID);
    
    void setGameID(const char *gameID)
    {
        this->gameID = gameID;
    }

    // CCSceneBase
    virtual void destruct();
    virtual void setup();
    virtual void deleteLater();

protected:
    virtual bool updateScene(const CCTime &time);
    virtual bool updateCamera(const CCTime &time);

public:
    virtual bool render(const CCCameraBase *inCamera, const int pass, const bool alpha);
    virtual void renderOctreeObject(CCObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha);
    
    virtual void beginOrientationUpdate();
    virtual void finishOrientationUpdate();

protected:
    virtual bool touchAllowed(const CCScreenTouches &touch);
    virtual bool handleOneTouch(const CCScreenTouches &touch1);
    virtual bool handleTwoTouches(const CCScreenTouches &touch1, const CCScreenTouches &touch2);
    
    virtual void touchRegistered(const CCScreenTouches &touch);
    virtual void twoTouchRegistered(const CCScreenTouches &touch1, const CCScreenTouches &touch2);
    
    virtual bool touchPressed(const CCScreenTouches &touch);
    virtual bool touchMovementAllowed(const CCScreenTouches &touch, CCPoint &touchDelta);
    virtual bool touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta);
    virtual bool touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction);
    
    virtual bool touchCameraMoving(const CCScreenTouches &touch, const float x, const float y);
    virtual bool touchCameraZooming(const float amount);
    virtual bool touchCameraRotating(const float x, const float y);
    virtual bool touchReleaseSwipe(const CCScreenTouches &touch);
    
    virtual void refreshCameraView();
public:
    virtual void lockCameraView();
    
public:
    const char* getGameID() { return gameID.buffer; }
    bool isValidGameID() { return gameID.length > 0; }
    void setFrameBufferID(const int frameBufferID);
    const int getFrameBufferID() { return frameBufferID; }
    
protected:
    virtual void createEnvironment();
    void setupPlayerDestinationIndicator(const char *textureFile, CCColour colour);
    
public:
    virtual void adjustCollisionPlacement(CCObjectCollideable *source);
    virtual CharacterPlayer* spawnCharacter(const char *playerID, const char *name);
    void deletingCharacter(CharacterPlayer *character);
    PickupBase* spawnPickup(const char *name, const char *type, const char *pickupID);
    
    void assignPlayerCharacter(CharacterPlayer *character);
    void addFriend(CharacterPlayer *character);
    void removeFriend(const char *playerID);
    CharacterPlayer* getPlayer(const char *playerID);
    CharacterPlayer* getFriend(CCObject *object);
    
    virtual void registerAttack(CCObject *from, CCObject *to, const float force);
    virtual void handlePickup(CharacterPlayer *player, PickupBase *pickup);
    
    void pause();
    void unPause();
    
    const int getNumberOfPlayers();
    CCPathFinderNetwork* getPathFinderNetwork()
    {
        return &pathFinderNetwork;
    }
    
    const CCSize& getMapBounds() { return mapBounds; }
    
    void setLocation(const char *playerID, const CCVector3 &location);
    
protected:
    int frameBufferID;
    
    CCText gameID;
    class SceneGameUI *sceneGameUI;
    
    // Ensures the two touch gesture we start with is used throughout
	bool oneTouchDoubleTapped, oneTouchDoubleTappedLastPress;
    CCTwoTouchAction twoTouchAction;
    CCVector3 cameraPanningFrom;
    
	CCPathFinderNetwork pathFinderNetwork;
    CCSize mapBounds;
    CollideableFloor *ground;

    CharacterPlayer *playerCharacter;
    CCVector3 *playerDestinationPending;
    ObjectIndicator *playerDestinationIndicator;
    
    CCList<CharacterPlayer> friends;
    
    SceneGameManager gameManager;
    CCList<CharacterPlayer> enemies;
    
    CCList<PickupBase> pickups;
};
