/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCSceneAppUI.h
 * Description : AppUI scene template.
 *
 * Created     : 06/06/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#include "CCCameraAppUI.h"

class CCTile3D;

class CCSceneAppUI : public CCSceneBase, public virtual CCLazyCallback
{
public:
    typedef CCSceneBase super;

	CCSceneAppUI();
	virtual void destruct();
    
    // CCSceneBase
	virtual void setup();
    
    // Called by the Engine to let the scene fetch and handle the controls
    virtual bool updateControls(const CCTime &time);
    
    // Called by updateControls or a user object perhaps rendering the scene in a frame buffer with specific touches to handle the controls with
    virtual bool handleTouches(const CCScreenTouches &touch1, const CCScreenTouches &touch2, const CCTime &time);
    
protected:
    virtual bool updateScene(const CCTime &time);
    virtual bool updateCamera(const CCTime &time);
    
public:
    virtual bool render(const CCCameraBase *inCamera, const int pass, const bool alpha);
	virtual void renderOctreeObject(CCObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha);

    virtual void render2DBackground(const uint inCameraIndex);
    virtual void render2DForeground(const uint inCameraIndex);
    
    virtual void beginOrientationUpdate();
    virtual void finishOrientationUpdate();

public:
    void addTile(CCTile3D *tile, const int index=-1)
    {
        tiles.add( tile );
        if( index >= 0 )
        {
            tiles.reinsert( tile, index );
        }
    }
    
    CCTile3D* getTile(const int i)
    {
        return tiles.list[i];
    }
    
protected:
    virtual void cameraUpdated() {}
    
    virtual bool touchAllowed(const CCScreenTouches &touch);
    virtual bool handleOneTouch(const CCScreenTouches &touch1);
    virtual bool handleTwoTouches(const CCScreenTouches &touch1, const CCScreenTouches &touch2);
    virtual bool handleThreeTouches(const CCScreenTouches &touch1,
                                          const CCScreenTouches &touch2, 
                                          const CCScreenTouches &touch3);
    
    virtual void touchRegistered(const CCScreenTouches &touch);
    virtual void twoTouchRegistered(const CCScreenTouches &touch1, const CCScreenTouches &touch2);
    virtual bool touchPressed(const CCScreenTouches &touch);
    virtual bool touchMovementAllowed(const CCScreenTouches &touch, CCPoint &touchDelta);
    virtual bool touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta);
    virtual bool touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction);
    
    enum CCTileTouchResult
    {
        tile_touchNone,
        tile_touchCollision,
        tile_touchAction,
    };
    virtual CCTileTouchResult handleTilesTouch(const CCScreenTouches &touch, const CCTouchAction touchAction);

    virtual bool touchCameraMoving(const CCScreenTouches &touch, const float x, const float y);
    virtual bool touchCameraZooming(const float amount);
    virtual bool touchCameraRotating(const float x, const float y);
    virtual bool touchReleaseSwipe(const CCScreenTouches &touch);
    
    virtual void refreshCameraView();
public:
    virtual void lockCameraView();
    CCCameraAppUI* getCamera() { return camera; }
    virtual void scrollCameraToTop();
    
protected:
    CCCameraAppUI *camera;
    bool cameraCentered;            // Camera centered in the middle or top of view?
    bool cameraScrolling;
    float sceneLeft, sceneRight;
    float sceneTop, sceneBottom;    // The top and bottom in relation to the center of the camera

    LAMBDA_SIGNAL orientationCallbacks;
    bool updatingOrientation;
    
	void *controlsUITouch;
    bool controlsMoving;
    bool controlsMovingVertical;
    
    CCList<CCTile3D> tiles;             // Used in the handleTilesTouch function to detect touches
};
