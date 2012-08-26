/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCObject.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCObjects.h"
#include "CCSceneBase.h"


CCObject::CCObject()
{
    enabled = true;
    inScene = NULL;
    deleteMe = 0;
	
	parent = NULL;
	
	model = NULL;

    renderPass = render_main;
    octreeRender = false;
    readDepth = true;
    writeDepth = true;
    disableCulling = false;
    
    transparent = false;
    transparentParent = false;
}


void CCObject::destruct()
{
    if( inScene != NULL )
    {
        removeFromScene();
    }
    else if( parent == NULL )
    {
        ASSERT( false );
    }
    else
    {
        parent->removeChild( this );
    }

    updaters.deleteObjectsAndList();
    children.deleteObjectsAndList();

    DELETE_OBJECT( model );

    super::destruct();
}


// RenerableBase
void CCObject::dirtyWorldMatrix()
{
    super::dirtyWorldMatrix();

    for( int i=0; i<children.length; ++i )
    {
        children.list[i]->dirtyWorldMatrix();
    }
}


void CCObject::setScene(CCSceneBase *scene)
{
    scene->addObject( this );
}


void CCObject::removeFromScene()
{
	deactivate();
    inScene->removeObject( this );
}


void CCObject::deleteLater()
{
    // Mark to delete in 2 frames
    deleteMe = 2;
    deactivate();
}


void CCObject::addChild(CCObject *object)
{
    children.add( object );

    object->parent = this;

    if( transparentParent == false )
    {
        if( object->transparentParent )
        {
            setTransparentParent( true );
        }
    }
}


bool CCObject::removeChild(CCObject *object)
{
    if( children.remove( object ) )
    {
        if( children.length == 0 )
        {
            children.freeList();
        }

        return true;
    }

    return false;
}


void CCObject::moveChildToScene(CCObject *object, CCSceneBase *scene)
{
    object->translate( &position );
    object->setScene( scene );
    ASSERT( removeChild( object ) );
}


void CCObject::addUpdater(CCUpdater *updater)
{
    updaters.add( updater );
}


void CCObject::removeUpdater(CCUpdater *updater)
{
    updaters.remove( updater );
}


bool CCObject::shouldCollide(CCObjectCollideable *collideWith, const bool initialCall)
{
    // It's not me is it?
    if( this == collideWith )
    {
        return false;
    }

    // Ask my parent if we should collide
    if( parent != NULL )
    {
        return parent->shouldCollide( collideWith, initialCall );
    }

    // Yeah, let's collide baby
    return true;
}


bool CCObject::update(const CCTime &time)
{
    bool updated = false;
    
    for( int i=0; i<updaters.length; ++i )
    {
        updated |= updaters.list[i]->update( time.delta );
    }

    for( int i=0; i<children.length; ++i )
    {
        updated |= children.list[i]->update( time );
    }
    
    return updated;
}


void CCObject::renderObject(const CCCameraBase *camera, const bool alpha)
{	
    if( shouldRender )
    {
        if( alpha == false || transparentParent )
        {
            GLPushMatrix();
            {
                refreshModelMatrix();
                GLMultMatrixf( modelMatrix );

                if( alpha == transparent )
                {
                    renderModel( alpha );
                }
                
                for( int i=0; i<children.length; ++i )
                {
                    children.list[i]->renderObject( camera, alpha );
                }
            }
            GLPopMatrix();
        }
    }
}


void CCObject::renderModel(const bool alpha)
{
    if( model )
    {
        if( disableCulling )
        {
            glDisable( GL_CULL_FACE );
        }
        
        if( alpha )
        {
            if( readDepth )
            {
                GLEnableDepth();
                if( writeDepth )
                {
                    model->render( alpha );
                }
                else 
                {
                    glDepthMask( GL_FALSE );
                    model->render( alpha );
                    glDepthMask( GL_TRUE );
                }
                GLDisableDepth();
            }
            else
            {
                model->render( alpha );
            }
        }
        else
        {
            if( readDepth )
            {
                if( writeDepth )
                {
                    model->render( alpha );
                }
                else 
                {
                    glDepthMask( GL_FALSE );
                    model->render( alpha );
                    glDepthMask( GL_TRUE );
                }
            }
            else
            {
                GLDisableDepth();
                model->render( alpha );
                GLEnableDepth();   
            }
        }
        
        if( disableCulling )
        {
            glEnable( GL_CULL_FACE );
        }
    }
}


void CCObject::setTransparent(const bool toggle)
{
    transparent = toggle;
    setTransparentParent( toggle );
    
    if( toggle )
    {
        readDepth = false;
    }
    else
    {
        readDepth = true;
    }
}


void CCObject::setTransparentParent(const bool toggle)
{
    transparentParent = toggle;
    if( parent != NULL )
    {
        parent->setTransparentParent( toggle );
    }
}

