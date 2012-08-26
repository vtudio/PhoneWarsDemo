/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCSceneBase.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCSceneBase.h"
#include "CCObjects.h"


CCSceneBase::CCSceneBase()
{
	enabled = true;
    deleteMe = false;
	
    objects.allocate( MAX_OBJECTS );
	
    parentScene = NULL;
	lifetime = 0.0f;
}


void CCSceneBase::destruct()
{
    childScenes.deleteObjectsAndList();

    while( objects.length > 0 )
    {
        CCObject *object = objects.list[0];
        DELETE_OBJECT( object );
    }
    ASSERT( collideables.length == 0 );
    
    if( parentScene != NULL )
    {
        parentScene->deletingChild( this );
    }
}


void CCSceneBase::deleteLater()
{
    for( int i=0; i<collideables.length; ++i )
    {
        CCObjectCollideable *collideable = collideables.list[i];
        collideable->collideableType = collision_none;
    }
    
    deleteMe = true;
    deleteLinkedScenesLater();
    
    if( parentScene != NULL )
    {
        parentScene->deletingChild( this );
    }
}


void CCSceneBase::deleteLinkedScenesLater()
{
    for( int i=0; i<linkedScenes.length; ++i )
    {
        CCSceneBase *linkedScene = linkedScenes.list[i];
        linkedScene->unlinkScene( this );
        linkedScene->deleteLater();
    }
    linkedScenes.length = 0;
}


bool CCSceneBase::updateControls(const CCTime &time)
{
    for( int i=0; i<childScenes.length; ++i )
    {
        if( childScenes.list[i]->updateControls( time ) )
        {
            return true;
        }
    }
	return false;
}


bool CCSceneBase::update(const CCTime &time)
{
    bool updated = false;
    
    // Remove any redundant scenes
    for( int i=0; i<childScenes.length; ++i )
    {
        CCSceneBase *scene = childScenes.list[i];
        if( scene->shouldDelete() )
        {
            removeChildScene( scene );
            i--;
            updated = true;
        }
    }

    if( enabled && deleteMe == false )
    {
        updated |= updateTask( time );
    }
    
    return updated;
}


bool CCSceneBase::updateTask(const CCTime &time)
{
    lifetime += time.real;
    
    bool updated = updateScene( time );
    
    // Update our child scenes
    for( int i=0; i<childScenes.length; ++i )
    {
        CCSceneBase *scene = childScenes.list[i];
        updated |= scene->update( time );
    }
    
    updateCamera( time );
    for( int i=0; i<childScenes.length; ++i )
    {
        updated |= childScenes.list[i]->updateCamera( time );
    }
    
    return updated;
}


bool CCSceneBase::updateScene(const CCTime &time)
{
    bool updated = false;
    
    for( int i=0; i<objects.length; ++i )
    {
        CCObject *object = objects.list[i];
        if( object->isActive() )
        {
            updated |= object->update( time );
        }
        else if( object->deleteMe > 0 )
        {
            if( --object->deleteMe == 0 )
            {
                DELETE_OBJECT( object );
                --i;
            }
        }
    }
    
    return updated;
}


bool CCSceneBase::updateCamera(const CCTime &time)
{
    return false;
}


bool CCSceneBase::render(const CCCameraBase *inCamera, const int pass, const bool alpha)
{
    bool rendered = false;
    if( inCamera->getIndex() == 0 )
	{
        renderObjects( inCamera, pass, alpha );
        rendered = true;
	}

    for( int i=0; i<childScenes.length; ++i )
    {
        rendered |= childScenes.list[i]->render( inCamera, pass, alpha );
    }

    return rendered;
}


void CCSceneBase::renderObjects(const CCCameraBase *inCamera, const int pass, const bool alpha)
{
#if defined PROFILEON
    CCProfiler profile( "CCSceneBase::renderObjects()" );
#endif

    for( int i=0; i<objects.length; ++i )
    {
        CCObject *object = objects.list[i];
        if( object->renderPass == pass )
        {
            if( object->isActive() && ( object->octreeRender == false ) )
            {
                object->renderObject( inCamera, alpha );
            }
        }
    }
}


void CCSceneBase::renderOctreeObject(CCObject *object, const CCCameraBase *inCamera, const int pass, const bool alpha)
{
	if( inCamera->getIndex() == 0 )
	{
        object->renderObject( inCamera, alpha );
	}
}


void CCSceneBase::render2DBackground(const uint inCameraIndex)
{
    for( int i=0; i<childScenes.length; ++i )
    {
        childScenes.list[i]->render2DBackground( inCameraIndex );
    }
}


void CCSceneBase::render2DForeground(const uint inCameraIndex)
{
    for( int i=0; i<childScenes.length; ++i )
    {
        childScenes.list[i]->render2DForeground( inCameraIndex );
    }
}


void CCSceneBase::addObject(CCObject *object)
{
    ASSERT( object->inScene == NULL );
    object->inScene = this;
    objects.add( object );
    ASSERT( objects.length < MAX_OBJECTS );
}


void CCSceneBase::removeObject(CCObject* object)
{
    //DEBUGLOG( "removeObject - %@", [object description] );
	
    ASSERT( object->inScene == this );

    object->inScene = NULL;
    const bool removed = objects.remove( object );
    if( removed == false )
    {
        ASSERT( removed );
    }
}


void CCSceneBase::addCollideable(CCObjectCollideable *collideable)
{
    collideables.add( collideable );
	gEngine->addCollideable( collideable );
}


void CCSceneBase::removeCollideable(CCObjectCollideable *collideable)
{
    collideables.remove( collideable );
	gEngine->removeCollideable( collideable );
}


void CCSceneBase::setParent(CCSceneBase *inParent)
{
    ASSERT( parentScene == NULL );
    parentScene = inParent;
}


void CCSceneBase::addChildScene(CCSceneBase *inScene)
{
    inScene->setParent( this );
    childScenes.add( inScene );
    inScene->setup();
}


void CCSceneBase::removeChildScene(CCSceneBase *inScene)
{
    if( childScenes.remove( inScene ) )
    {
        DELETE_OBJECT( inScene );
    }
}


void CCSceneBase::linkScene(CCSceneBase *inScene)
{
    linkedScenes.add( inScene );
}


void CCSceneBase::unlinkScene(CCSceneBase *inScene)
{
    linkedScenes.remove( inScene );
}
