/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCTextureBase.cpp
 *-----------------------------------------------------------
 */

#include "CCTextureBase.h"


CCTextureBase::CCTextureBase()
{
	glName = 0;
}


CCTextureBase::~CCTextureBase()
{
	if( glName != 0 )
	{
		glDeleteTextures( 1, &glName );
	}
}


bool CCTextureBase::loadAndCreate(const char *path, const CCResourceType resourceType, const bool generateMipMap)
{
    if( !load( path, resourceType, generateMipMap ) || !createGLTexture() )
	{
		return false;
	}
	
	return true;
}


bool CCTextureBase::createGLTexture()
{
	return false;
}


bool CCTextureBase::extensionSupported(const char *extension)
{
    const GLubyte *extensions = NULL;
    const GLubyte *start;
    GLubyte *where, *terminator;

    // Extension names should not have spaces
    where = (GLubyte*)strchr( extension, ' ' );
    if( where || *extension == '\0' ) return 0;

    // It takes a bit of care to be fool-proof about parsing the OpenGL extensions string. Don't be fooled by sub-strings, etc.
    extensions = glGetString( GL_EXTENSIONS );
    start = extensions;
    for(;;)
    {
        where = (GLubyte*)strstr( (const char*)start, extension );
        if( !where )
        {
            break;
        }
        terminator = where + strlen( extension );
        if( where == start || *( where - 1 ) == ' ' )
        {
            if( *terminator == ' ' || *terminator == '\0' )
            {
                return true;
            }
        }
        start = terminator;
    }
    return false;
}
