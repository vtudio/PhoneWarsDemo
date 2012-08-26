/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCEngine.h
 * Description : Handles the update and render loop and feature managers.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCENGINE_H__
#define __CCENGINE_H__


struct CCTime
{
	CCTime()
	{
		real = 0.0f;
		delta = 0.0f;
        lifetime = 0.0f;
        lastUpdate = 0;
	}
	
    float real;
	float delta;
	float lifetime;

    double lastUpdate;
};


#include "CCControls.h"
#include "CCCameraBase.h"
#include "CCTextureManager.h"
#include "CCOctree.h"
#include "CCRenderer.h"

class CCSceneBase;
class CCObjectCollideable;

class CCEngine
{
public:
	CCEngine();
	virtual ~CCEngine();
	
public:
	virtual void setupNativeThread();
    virtual bool setupEngineThread();
	
    bool setupRenderer();
	
    void addCamera(CCCameraBase *camera, const int index=-1);
    bool removeCamera(CCCameraBase *camera);
    void refreshCameras();
	
	void addScene(CCSceneBase *scene, const int index=-1);
protected:
    // Don't call this directly, use deleteLater in scene
    void removeScene(CCSceneBase *scene);
	
    virtual bool updateTime();
public:
    virtual bool updateNativeThread();
	virtual void updateEngineThread();
    virtual bool updateJobsThread();
    
protected:
	virtual void start() = 0;
	virtual void updateLoop();
	void renderLoop();
    
public:
    void renderFrameBuffer(const int frameBufferID);
    
protected:
	// Finishes a job on the engine thread
	virtual void finishJobs();
    
public:
    virtual void restart();
	
	void addCollideable(CCObjectCollideable* collideable);
	void removeCollideable(CCObjectCollideable* collideable);
	
    virtual bool serialize(const bool saving=false);
    virtual bool isOrientationSupported(const float angle);
    
    // Run on another thread
    void nextEngineUpdate(CCLambdaCallback *lambdaCallback, const int index=-1);
    void engineToNativeThread(CCLambdaCallback *lambdaCallback);
    void nativeToEngineThread(CCLambdaCallback *lambdaCallback);
    void engineToJobsThread(CCLambdaCallback *lambdaCallback, const bool pushToFront=false);
    void jobsToEngineThread(CCLambdaCallback *lambdaCallback);
	
    void pause();
    void resume();
    
public:
	bool running, paused, engineThreadRunning;
    bool pauseRendering;
    
	CCRenderer *renderer;
	CCControls *controls;
	CCTextureManager *textureManager;
	
    CCList<CCCameraBase> cameras;
    CCDestructList<CCSceneBase> scenes;
	
    // Our Octree collideables container
	CCCollisionManager collisionManager;

    // Engine level controls used for timers and such
    CCDestructList<CCUpdater> updaters;
	
	CCTime time;
	uint renderFlags;
	float fpsLimit;
	
protected:
    CCList<CCLambdaCallback> nativeThreadCallbacks;
    CCList<CCLambdaCallback> engineThreadCallbacks;
    CCList<CCLambdaCallback> jobsThreadCallbacks;
};


#define LAMBDA_RUN_NATIVETHREAD(FUNCTION_CALL)                      \
LAMBDA_UNSAFE( ThreadCallback,                                      \
    FUNCTION_CALL                                                   \
)                                                                   \
gEngine->engineToNativeThread( new ThreadCallback() );


#define LAMBDA_RUN_ENGINETHREAD(FUNCTION_CALL)                      \
LAMBDA_UNSAFE( ThreadCallback,                                      \
   FUNCTION_CALL                                                    \
)                                                                   \
gEngine->nativeToEngineThread( new ThreadCallback() );              \

#define LAMBDA_CONNECT_NATIVETHREAD(EVENT, FUNCTION_CALL)           \
LAMBDA_UNSAFE( EventCallback,                                       \
    LAMBDA_UNSAFE( ThreadCallback,                                  \
        FUNCTION_CALL                                               \
    )                                                               \
    gEngine->engineToNativeThread( new ThreadCallback() );          \
)                                                                   \
EVENT = new EventCallback();


#define LAMBDA_CONNECT_ENGINETHREAD(EVENT, FUNCTION_CALL)           \
LAMBDA_UNSAFE( EventCallback,                                       \
    LAMBDA_UNSAFE( ThreadCallback,                                  \
        FUNCTION_CALL                                               \
    )                                                               \
    gEngine->nativeToEngineThread( new ThreadCallback() );          \
)                                                                   \
EVENT = new EventCallback();


#endif // __CCENGINE_H__
