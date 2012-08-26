/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCTextureManager.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCTexturePNG.h"
#include "CCTextureFontPageFile.h"


CCTextureHandle::~CCTextureHandle()
{
    onLoad.deleteObjectsAndList();
    delete texture;
}



CCTextureManager::CCTextureManager()
{
	currentGLTexture = 0;
    totalTexturesLoaded = 0;
    totalUsedTextureSpace = 0;
    totalTexturesLoadedThisFrame = 0;
	
	CCDefaultTexCoords();
}


CCTextureManager::~CCTextureManager()
{
	textureHandles.deleteObjectsAndList();
	
    fontPages.deleteObjectsAndList();
}


static float textureLoadTimeOut;
static uint textureLoadLimit;
void CCTextureManager::prepareRender()
{
    bool cameraMoving = false;
    for( int i=0; i<gEngine->cameras.length; ++i )
    {
        if( gEngine->cameras.list[i]->hasUpdated() )
        {
            cameraMoving = true;
            break;
        }
    }
    
    if( cameraMoving )
    {
        textureLoadTimeOut = 0.25f;
        textureLoadLimit = 1;
    }
    else
    {
        textureLoadTimeOut = 0.05f;
        textureLoadLimit = 2;
    }
    
    textureLoadingTimeout -= gEngine->time.real;
    if( textureLoadingTimeout <= 0.0f )
    {
        totalTexturesLoadedThisFrame = 0;
    }
}


void CCTextureManager::load()
{
    // Load in a 1x1 white texture to use for untextured draws
    assignTextureIndex( "Resources/Textures/white.png", Resource_Packaged, false, true, true );
    setTextureIndex( 0 );
}


void CCTextureManager::unloadAllTextures()
{
    for( int i=0; i<textureHandles.length; ++i )
    {
        CCTextureHandle *handle = textureHandles.list[i];
        if( handle->texture != NULL )
        {
            totalTexturesLoaded--;
            totalUsedTextureSpace -= handle->texture->getBytes();
            delete handle->texture;
            handle->texture = NULL;
        }
    }
}


void CCTextureManager::loadFont(const char *font, const char *path)
{
    CCText file = path;
    file += font;
    
    CCTextureFontPageFile *fontPage = NULL;
    for( int i=0; i<fontPages.length; ++i )
    {
        if( CCText::Equals( font, fontPages.list[i]->getName() ) )
        {
            fontPage = (CCTextureFontPageFile*)fontPages.list[i];
            break;
        }
    }
    
    if( fontPage == NULL )
    {
        fontPage = new CCTextureFontPageFile( font );
        fontPage->load( file.buffer, Resource_Packaged, true );
        fontPages.add( fontPage );
    }
}


uint CCTextureManager::assignTextureIndex(const char *filePath, const CCResourceType resourceType,
                                                const bool mipmap, const bool load, const bool alwaysResident)
{
    CCTextureHandle *handle = NULL;
    for( int i=0; i<textureHandles.length; ++i )
    {
        CCTextureHandle *textureHandleItr = textureHandles.list[i];
        if( strcmp( textureHandleItr->filePath.buffer, filePath ) == 0 )
        {
            handle = textureHandleItr;
            if( load && handle->texture == NULL )
            {
                break;
            }
            else
			{
				return i;
			}
		}
    }

    if( handle == NULL )
    {
        handle = new CCTextureHandle( filePath, resourceType );
        textureHandles.add( handle );
    }
    
    if( mipmap )
    {
        handle->mipmap = mipmap;
    }

    // By default all textures don't hang around
    if( alwaysResident == true || load )
    {
        handle->alwaysResident = true;
        loadTexture( *handle );
    }
    
    uint index = (uint)( textureHandles.length-1 );
    return (uint)index;
}


bool CCTextureManager::loadTexture(CCTextureHandle &textureHandle)
{
#if defined PROFILEON
    CCProfiler profile( "CCTextureManager::loadTexture()" );
#endif

	CCTextureBase *texture = new CCTexturePNG();
    if( texture->loadAndCreate( textureHandle.filePath.buffer, textureHandle.resourceType, textureHandle.mipmap ) == false )
	{
        //ASSERT( false );
		delete texture;
		texture = NULL;
        textureHandle.loadable = false;
	}
	
	if( texture != NULL )
    {
        totalTexturesLoaded++;
        totalUsedTextureSpace += texture->getBytes();
        
        // Estimated texture usage, need to cater for bit depth and mip maps for more accuracy
        const uint maxSpace = 72 * 1024 * 1024;
        if( totalUsedTextureSpace > maxSpace )
        {
            //DEBUGLOG( "CCTextureManager::Textures Loaded %i %i \n", totalTexturesLoaded, totalTextureSpace );
            const uint targetSpace = 64 * 1024 * 1024;
            while( totalUsedTextureSpace > targetSpace )
            {
                float oldestTime = -1.0f;
                CCTextureHandle *oldestHandle = NULL;
                for( int i=1; i<textureHandles.length; ++i )
                {
                    CCTextureHandle *handle = textureHandles.list[i];
                    //DEBUGLOG( "handle: %s \n", handle->filePath.buffer );
                    if( handle != NULL )
                    {
                        if( handle->alwaysResident == false )
                        {
                            if( handle->texture != NULL )
                            {
                                const float distance = gEngine->time.lifetime - handle->lastTimeUsed;
                                if( distance > oldestTime )
                                {
                                    oldestTime = distance;
                                    oldestHandle = handle;
                                }
                            }
                        }
                    }
                }

                if( oldestHandle == NULL )
                {
#ifdef DEBUGON
                    // Try again for debugging
                    for( int i=1; i<textureHandles.length; ++i )
                    {
                        CCTextureHandle *handle = textureHandles.list[i];
                        //DEBUGLOG( "handle: %s \n", handle->filePath.buffer );
                        if( handle != NULL )
                        {
                            if( handle->alwaysResident == false )
                            {
                                if( handle->texture != NULL )
                                {
                                    const float distance = gEngine->time.lifetime - handle->lastTimeUsed;
                                    if( distance > oldestTime )
                                    {
                                        oldestTime = distance;
                                        oldestHandle = handle;
                                    }
                                }
                            }
                        }
                    }
#endif
                    ASSERT( oldestHandle != NULL );
                }
                
                if( oldestHandle != NULL )
                {
                    totalTexturesLoaded--;
                    totalUsedTextureSpace -= oldestHandle->texture->getBytes();
                    delete oldestHandle->texture;
                    oldestHandle->texture = NULL;
                    //DEBUGLOG( "CCTextureManager::Textures Loaded %i %i \n\n", totalTexturesLoaded, totalTextureSpace );
                }
            }
        }
        textureHandle.texture = texture;
        return true;
    }
    
    return false;
}


void CCTextureManager::bindTexture(const uint glName)
{
	if( currentGLTexture != glName )
	{
		glBindTexture( GL_TEXTURE_2D, glName );
		currentGLTexture = glName;
	}
}


bool CCTextureManager::setTextureIndex(const int handleIndex)
{
    CCTextureHandle *handle = textureHandles.list[handleIndex];
    if( handle != NULL && handle->loadable )
    {
        if( handle->texture == NULL )
        {
            if( totalTexturesLoadedThisFrame >= textureLoadLimit || loadTexture( *handle ) == false )
            {
                setTextureIndex( 0 );
                return false;
            }
            totalTexturesLoadedThisFrame++;
            textureLoadingTimeout = textureLoadTimeOut;

            LAMBDA_EMIT_ONCE( handle->onLoad );
        }

        handle->lastTimeUsed = gEngine->time.lifetime;
        bindTexture( handle->texture->glName );
        return true;
	}
	else
	{
		setTextureIndex( 0 );
		return false;
	}	
}


CCTextureHandle* CCTextureManager::getTextureHandle(const int handleIndex)
{
    return textureHandles.list[handleIndex];
}


const CCTextureBase* CCTextureManager::getTexture(const int handleIndex)
{
    CCTextureHandle *handle = textureHandles.list[handleIndex];
    if( handle != NULL )
    {
        if( handle->texture == NULL )
        {
            loadTexture( *handle );
            LAMBDA_EMIT_ONCE( handle->onLoad );
        }
        return handle->texture;
    }
    return NULL;
}


const CCTextureBase* CCTextureManager::getTexture(const char *filePath, const CCResourceType resourceType)
{
    const uint textureHandleIndex = assignTextureIndex( filePath, resourceType, false, false, false );
    const CCTextureBase *texture = getTexture( textureHandleIndex );
    return texture;
}


float CCTextureManager::getTextureWidth(const char *filePath, const CCResourceType resourceType)
{
    const uint textureHandleIndex = assignTextureIndex( filePath, resourceType, false, false, false );
    const CCTextureBase *texture = getTexture( textureHandleIndex );
    return texture->getImageWidth();
}
