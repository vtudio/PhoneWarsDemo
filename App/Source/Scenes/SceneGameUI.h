/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : SceneGameUI.h
 * Description : UI for our in game scene
 *
 * Created     : 29/04/12
 *-----------------------------------------------------------
 */

#ifndef __SCENEGAMEUI_H__
#define __SCENEGAMEUI_H__


class SceneGameUI : public CCSceneAppUI
{
public:
    typedef CCSceneAppUI super;

    SceneGameUI(SceneGameSyndicate *sceneGame);

    // CCSceneBase
	virtual void destruct();
	virtual void setup();
    
protected:
	virtual bool updateScene(const CCTime &time);
    virtual bool updateCamera(const CCTime &time);
    
public:
    virtual bool render(const CCCameraBase *inCamera, const int pass, const bool alpha);
	virtual void renderOctreeObject(CCObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha);
    
    virtual void beginOrientationUpdate();
    
protected:
    virtual bool touchPressed(const CCScreenTouches &touch);
    virtual bool touchMoving(const CCScreenTouches &touch, const CCPoint &touchDelta);
    virtual bool touchReleased(const CCScreenTouches &touch, const CCTouchAction touchAction);

    virtual bool touchCameraRotating(const float x, const float y);
    virtual bool touchReleaseSwipe(const CCScreenTouches &touch);
    
public:
    void setFrameBufferID(const int frameBufferID);
    
    void setHealthAlpha(const int index, float alpha=-1.0f);
    
protected:
    SceneGameSyndicate *sceneGame;
    
    CCTile3DButton *tileHealth[2];
};


#endif // __SCENEGAMEUI_H__
