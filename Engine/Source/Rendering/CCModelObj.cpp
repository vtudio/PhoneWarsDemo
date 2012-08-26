/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCModelObj.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCPrimitives.h"
#include "CCModelObj.h"
#include "CCFileManager.h"
#include "CCTextureBase.h"

#include "ObjLoader.h"


struct PrimitiveCache
{
    CCText file;
    CCPrimitiveObj *primitive;
};
static CCList<PrimitiveCache> primitivesCache;


CCModelObj* CCModelObj::CacheModel(const char *file, const char *texture, 
                                   const bool moveVerticesToOrigin, 
                                   const CCResourceType resourceType, 
                                   const bool mipmap, const bool load)
{
    CCModelObj *model = new CCModelObj();
    
    PrimitiveCache *cachedObj = NULL;
    for( int i=0; i<primitivesCache.length; ++i )
    {
        PrimitiveCache *cache = primitivesCache.list[i];
        if( moveVerticesToOrigin == cache->primitive->hasMovedToOrigin() )
        {
            if( CCText::Equals( cache->file.buffer, file ) )
            {
                cachedObj = cache;
                break;
            }
        }
    }
    
    if( cachedObj == NULL )
    {
        cachedObj = new PrimitiveCache();
        cachedObj->file = file;
        
        CCPrimitiveObj *primitive = new CCPrimitiveObj();
        primitive->load( file );
        if( moveVerticesToOrigin )
        {
            primitive->moveVerticesToOrigin();
        }
        cachedObj->primitive = primitive;
        
        primitivesCache.add( cachedObj );
    }
    
    CCPrimitiveObj *primitive = new CCPrimitiveObj();
    primitive->copy( cachedObj->primitive );
    
    if( texture != NULL )
    {
        primitive->setTexture( texture, resourceType, mipmap, true, load );
    }
    model->primitive = primitive;
    model->addPrimitive( primitive );
    
    return model;
}


CCModelObj::CCModelObj()
{
    // Rotate as front is back and back is front
    rotateY( 180.0f );
}


CCModelObj::CCModelObj(const char *file, const char *texture, 
                       const bool moveVerticesToOrigin, 
                       const CCResourceType resourceType, 
                       const bool mipmap, const bool load)
{
    primitive = new CCPrimitiveObj();
    primitive->load( file );
    
    if( moveVerticesToOrigin )
    {
        this->moveVerticesToOrigin();
    }
    
    if( texture != NULL )
    {
        primitive->setTexture( texture, resourceType, mipmap, true, load );
    }
    addPrimitive( primitive );
    
    // Rotate as front is back and back is front
    rotateY( 180.0f );
}



// CCPrimitiveObj
CCPrimitiveObj::CCPrimitiveObj()
{
    vertexCount = 0;
    
    modelUVs = NULL;
    textureUVs = NULL;
    
    width = height = depth = 0.0f;
    
    cached = false;
    movedToOrigin = false;
}


void CCPrimitiveObj::destruct()
{
    if( cached )
    {
        modelUVs = NULL;
        vertices = NULL;
        normals = NULL;
    }
    
    if( modelUVs != NULL )
    {
        free( modelUVs );   
    }
    
    if( textureUVs != NULL )
    {
        free( textureUVs );   
    }
    
    super::destruct();
}


const bool CCPrimitiveObj::load(const char *file)
{
    CCText fullFilePath;
    CCFileManager::GetFilePath( fullFilePath, file, Resource_Packaged );
    
    /* attempt to load the file */
    ObjMesh *objMesh = LoadOBJ( fullFilePath.buffer );
    if( objMesh != NULL )
    {
        for( uint i=0; i<objMesh->m_iNumberOfFaces; ++i )
        {   
            ObjFace *pf = &objMesh->m_aFaces[i];
            if( pf->m_iVertexCount >= 3 )
            {
                uint faceVertexCount = pf->m_iVertexCount;
                do
                {
                    vertexCount += 3;
                    faceVertexCount--;
                } while( faceVertexCount >= 3 );
            }
        }
        
        modelUVs = (float*)malloc( sizeof( float ) * vertexCount * 2 );
        vertices = (float*)malloc( sizeof( float ) * vertexCount * 3 );
        normals = (float*)malloc( sizeof( float ) * vertexCount * 3 );
        
        int uvIndex = 0;
        int vertexIndex = 0;
        for( uint i=0; i<objMesh->m_iNumberOfFaces; ++i )
        {   
            ObjFace *pf = &objMesh->m_aFaces[i];
            if( pf->m_iVertexCount < 3 )
            {
                continue;
            }
            
            uint vertexStartIterator = 1;
            uint vertexEndIterator = 3;
            do
            {
                // Convert GL_POLYGON to GL_TRIANGLES by reusing the first vert, with the others
                {
                    uint j=0;
                    // UVs
                    {
                        ObjTexCoord &texCoord = objMesh->m_aTexCoordArray[ pf->m_aTexCoordIndicies[j] ];
                        modelUVs[uvIndex+0] = texCoord.u;
                        modelUVs[uvIndex+1] = 1.0f - texCoord.v;
                    }
                    
                    // Vertices
                    {
                        ObjVertex &vertex = objMesh->m_aVertexArray[ pf->m_aVertexIndices[j] ];
                        vertices[vertexIndex+0] = vertex.x;
                        vertices[vertexIndex+1] = vertex.y;
                        vertices[vertexIndex+2] = vertex.z;
                        
                        mmX.consider( vertex.x );
                        mmY.consider( vertex.y );
                        mmZ.consider( vertex.z );
                    }
                    
                    // Normals
                    if( objMesh->m_aNormalArray != NULL )
                    {
                        ObjNormal &normal = objMesh->m_aNormalArray[ pf->m_aNormalIndices[j] ];
                        normals[vertexIndex+0] = normal.x;
                        normals[vertexIndex+1] = normal.y;
                        normals[vertexIndex+2] = normal.z;
                    }
                    
                    uvIndex += 2;
                    vertexIndex += 3;
                }
                
                for( uint j=vertexStartIterator; j<vertexEndIterator; ++j )
                {
                    // UVs
                	if( objMesh->m_aTexCoordArray != NULL )
                    {
                		const unsigned int index = pf->m_aTexCoordIndicies[j];
                		if( index < objMesh->m_iNumberOfTexCoords )
                		{
							ObjTexCoord &texCoord = objMesh->m_aTexCoordArray[index];
							modelUVs[uvIndex+0] = texCoord.u;
							modelUVs[uvIndex+1] = 1.0f - texCoord.v;
                		}
                    }
                    
                    // Vertices
                    {
                		const unsigned int index = pf->m_aVertexIndices[j];
                        ObjVertex &vertex = objMesh->m_aVertexArray[index];
                        vertices[vertexIndex+0] = vertex.x;
                        vertices[vertexIndex+1] = vertex.y;
                        vertices[vertexIndex+2] = vertex.z;
                        
                        mmX.consider( vertex.x );
                        mmY.consider( vertex.y );
                        mmZ.consider( vertex.z );
                    }
                    
                    // Normals
                    // TODO: Fix, currently buggy
//                    if( objMesh->m_aNormalArray != NULL )
//                    {
//                        unsigned int index = pf->m_aNormalIndices[j];
//                        ObjNormal &normal = objMesh->m_aNormalArray[index];
//                        normals[vertexIndex+0] = normal.x;
//                        normals[vertexIndex+1] = normal.y;
//                        normals[vertexIndex+2] = normal.z;
//                    }
                    
                    uvIndex += 2;
                    vertexIndex += 3;
                }
                
                vertexStartIterator++;
                vertexEndIterator++;
                
            } while( vertexEndIterator <= pf->m_iVertexCount );
        }
        
        DeleteOBJ( objMesh->m_iMeshID );
        
        width = mmX.size();
        height = mmY.size();
        depth = mmZ.size();
    }
    
    return vertexCount > 0;
}


void CCPrimitiveObj::adjustTextureUVs()
{
    const int textureHandleIndex = textureInfo->primaryIndex;
    CCTextureHandle *textureHandle = gEngine->textureManager->getTextureHandle( textureHandleIndex );
    const CCTextureBase *texture = textureHandle->texture;
    ASSERT( texture != NULL );
    if( texture != NULL )
    {
        const float width = texture->getImageWidth();
        const float height = texture->getImageHeight();
        const float allocatedWidth = texture->getAllocatedWidth();
        const float allocatedHeight = texture->getAllocatedHeight();
        
        const float xScale = width / allocatedWidth;
        const float yScale = height / allocatedHeight;
        
        if( textureUVs == NULL )
        {
            textureUVs = (float*)malloc( sizeof( float ) * vertexCount * 2 );
        }
        
        for( uint i=0; i<vertexCount; ++i )
        {
            const int uvIndex = i*2;
            const int x = uvIndex+0;
            const int y = uvIndex+1;
            
            textureUVs[x] = modelUVs[x] * xScale;
            textureUVs[y] = modelUVs[y] * yScale;
        }
    }
}


void CCPrimitiveObj::renderVertices(const bool textured)
{
    CCSetViewMatrix();
	GLVertexPointer( 3, GL_FLOAT, 0, vertices );
    CCSetVertexAttribute( ATTRIB_NORMAL, 3, GL_FLOAT, 0, normals, true );
    CCSetTexCoords( textureUVs != NULL ? textureUVs : modelUVs );

	glDrawArrays( GL_TRIANGLES, 0, vertexCount );
}


const CCMinMax CCPrimitiveObj::getYMinMaxAtZ(const float atZ) const
{
    CCMinMax mmYAtZ;
    
    for( uint i=0; i<vertexCount; ++i )
    {
        const uint index = i*3;
        float &y = vertices[index+1];
        float &z = vertices[index+2];
        
        if( z >= atZ )
        {
            mmYAtZ.consider( y );
        }
    }
    
    return mmYAtZ;
}


const CCVector3 CCPrimitiveObj::getOrigin()
{
    if( movedToOrigin == false )
    {
        origin.x = mmX.min + ( width * 0.5f );
        origin.y = mmY.min + ( height * 0.5f );
        origin.z = mmZ.min + ( depth * 0.5f );
    }
    return origin;
}


void CCPrimitiveObj::moveVerticesToOrigin()
{
    if( movedToOrigin == false )
    {
        const CCVector3 origin = getOrigin();
        
        mmX.reset();
        mmY.reset();
        mmZ.reset();
        
        for( uint i=0; i<vertexCount; ++i )
        {
            const uint index = i*3;
            float &x = vertices[index+0];
            float &y = vertices[index+1];
            float &z = vertices[index+2];
            
            x -= origin.x;
            y -= origin.y;
            z -= origin.z;
            
            mmX.consider( x );
            mmY.consider( y );
            mmZ.consider( z );
        }
        
        movedToOrigin = true;
    }
}


void CCPrimitiveObj::copy(const CCPrimitiveObj *primitive)
{
    vertexCount = primitive->vertexCount;
    
    modelUVs = primitive->modelUVs;
    vertices = primitive->vertices;
    normals = primitive->normals;
    width = primitive->width;
    height = primitive->height;
    depth = primitive->depth;
    mmX = primitive->mmX;
    mmY = primitive->mmY;
    mmZ = primitive->mmZ;
    
    cached = true;
    movedToOrigin = primitive->movedToOrigin;
    origin = primitive->origin;
}