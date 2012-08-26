/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : SceneGameBurgersDeathmatch.h
 * Description : Deathmatch implementation of the base game scene.
 *
 * Created     : 10/07/12
 *-----------------------------------------------------------
 */

class SceneGameBurgersDeathmatch : public SceneGameSyndicate
{
public:
    typedef SceneGameSyndicate super;

    SceneGameBurgersDeathmatch(const char *gameID);

    // CCSceneBase
    virtual void destruct();
    virtual void setup();
    virtual void deleteLater();

protected:
    virtual bool updateScene(const CCTime &time);
    virtual bool updateCamera(const CCTime &time);

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
    
protected:
    virtual void createEnvironment();
    
public:
    virtual CharacterPlayer* spawnCharacter(const char *playerID, const char *name);
    PickupBase* spawnPickup(const char *name, const char *type, const char *pickupID);
    
    void assignPlayerCharacter(CharacterPlayer *character);
    void addFriend(CharacterPlayer *character);
    
    void registerAttack(CCObject *from, CCObject *to, const float force);
};
