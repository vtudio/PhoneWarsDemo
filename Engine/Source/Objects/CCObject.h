/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCObject.h
 * Description : A scene managed object.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

class CCObject : public CCRenderable
{
public:
	typedef CCRenderable super;
	
    CCObject();
    virtual void destruct();

    // CCRenderable
    virtual void dirtyWorldMatrix();

    virtual void setScene(CCSceneBase *scene);
    virtual void removeFromScene();

    // Delete the object in 2 frames.
    void deleteLater();

    inline bool isActive() { return deleteMe == 0; }
    virtual void deactivate() {};

    void addChild(CCObject *object);
    bool removeChild(CCObject *object);

    // Remove an object from our child list and add it into the scene
    void moveChildToScene(CCObject *object, CCSceneBase *scene);

    void addUpdater(CCUpdater *updater);
    void removeUpdater(CCUpdater *updater);

    virtual bool shouldCollide(CCObjectCollideable *collideWith, const bool initialCall);

    virtual bool update(const CCTime &time);
    virtual void renderObject(const CCCameraBase *camera, const bool alpha);
    
protected:
    virtual void renderModel(const bool alpha);

public:
    bool isTransparent() { return transparent; }
    void setTransparent(const bool toggle=true);
    void setTransparentParent(const bool toggle=true);

    bool enabled;               // Ambigious: Currently controls if collision handing for line checks are enabled
    CCSceneBase *inScene;
    uint deleteMe;

    CCObject *parent;

    CCModelBase *model;

    CCRenderPass renderPass;
    bool octreeRender;
    bool readDepth, writeDepth;
    bool disableCulling;

protected:
    bool transparent, transparentParent;

    // If I have children, they will be offset from my position
    CCDestructList<CCObject> children;

    CCDestructList<CCUpdater> updaters;
};
