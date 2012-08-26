/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCSceneBase.h
 * Description : Handles the drawing and updating of objects.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCSCENEBASE_H__
#define __CCSCENEBASE_H__


class CCObject;
class CCWidgetBase;
class CCWidgetButton;

class CCSceneBase : public CCBaseType
{
public:
	CCSceneBase();
	virtual void destruct();
	
	virtual void setup() {};
	virtual void restart() {}
	
    virtual void deleteLater();
    void deleteLinkedScenesLater();
    inline bool shouldDelete() { return deleteMe; }
	
    // Called by the Engine to let the scene fetch and handle the controls
    virtual bool updateControls(const CCTime &time);
    
    // Called by updateControls or a user object perhaps rendering the scene in a frame buffer with specific touches to handle the controls with
    virtual bool handleTouches(const CCScreenTouches &touch1, const CCScreenTouches &touch2, const CCTime &time) { return false; }
    
    virtual bool handleBackButton() { return false; }
    
    bool update(const CCTime &time);
    bool updateTask(const CCTime &time);
    
protected:
    virtual bool updateScene(const CCTime &time);
    virtual bool updateCamera(const CCTime &time);
	
public:
    virtual bool render(const CCCameraBase *inCamera, const int pass, const bool alpha);
    
protected:
    virtual void renderObjects(const CCCameraBase *inCamera, const int pass, const bool alpha);
    
public:
	// For sorted objects, we get passed the object to draw here
	virtual void renderOctreeObject(CCObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha);

    virtual void render2DBackground(const uint inCameraIndex);
    virtual void render2DForeground(const uint inCameraIndex);
	
	// Add object to the scene and place in the created list
	void addObject(CCObject *object);
	void removeObject(CCObject* object);
    
    void addCollideable(CCObjectCollideable *collideable);
    void removeCollideable(CCObjectCollideable *collideable);
    
    // Tells the scene to inform on deletes
    void setParent(CCSceneBase *inParent);
    virtual void deletingChild(CCSceneBase *inScene) {}
    
    // Passes on renders and controls to child scenes
public:
    void addChildScene(CCSceneBase *inScene);
    void removeChildScene(CCSceneBase *inScene);
    
    // Linked scenes are deleted along with this scene
    void linkScene(CCSceneBase *inScene);
    void unlinkScene(CCSceneBase *inScene);

    virtual void beginOrientationUpdate() {}
    virtual void finishOrientationUpdate() {}
	
    // Runs on native thread
    virtual void appPaused() {}
    virtual void appResumed() {}
    
public:
    bool enabled;
    
protected:
    bool deleteMe;

protected:
    CCList<CCObject> objects;
    CCList<CCObjectCollideable> collideables;

    CCSceneBase *parentScene;
    CCList<CCSceneBase> childScenes;
    CCList<CCSceneBase> linkedScenes;

    float lifetime;
};


#endif // __CCSCENEBASE_H__
