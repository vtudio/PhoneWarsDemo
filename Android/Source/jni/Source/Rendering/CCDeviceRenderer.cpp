/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCDeviceRenderer.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCDeviceRenderer.h"
#include "CCGLView.h"
#include "CCFileManager.h"


static bool compileShader(GLuint *shader, GLenum type, const char *path)
{
	GLint status;
	
	char *data;
	const int fileSize = CCFileManager::GetPackagedFile( path, &data );

	const int offset = 41;
	char code[fileSize + offset];
	
	if( type == GL_VERTEX_SHADER )
	{
		sprintf( code, "%s%s", "#define VERTEX_SHADER\r\n", data );
	}
	else if( type == GL_FRAGMENT_SHADER )
	{
		sprintf( code, "%s%s", "#define PIXEL_SHADER\r\n", data );
	}
	
	// For safety
	code[fileSize + offset - 1] = '0';
	const char* appendedCode = (const char*)&code[0];
	
	*shader = glCreateShader( type );
    DEBUG_OPENGL();
    
	glShaderSource( *shader, 1, &appendedCode, NULL );
	DEBUG_OPENGL();
    
	glCompileShader( *shader );
	DEBUG_OPENGL();

#ifdef DEBUGON
	GLint logLength = 0;
	glGetShaderiv( *shader, GL_INFO_LOG_LENGTH, &logLength );
	if( logLength > 0 )
	{
		char *log = (char*)malloc( logLength );
		glGetShaderInfoLog( *shader, logLength, &logLength, log );
		DEBUGLOG( "Shader compile log:\n%s", log );
		free( log );
	}
#endif
	
	glGetShaderiv( *shader, GL_COMPILE_STATUS, &status );
	if( status == 0 )
	{
		glDeleteShader( *shader );
		return false;
	}
	
	return true;
}


CCDeviceRenderer::~CCDeviceRenderer()
{
    frameBufferManager.destoryAllFrameBuffers();
}


bool CCDeviceRenderer::linkProgram(GLuint &program)
{
    glLinkProgram( program );
    GLint linkStatus = GL_FALSE;
    glGetProgramiv( program, GL_LINK_STATUS, &linkStatus );
    if( linkStatus != GL_TRUE )
    {
        GLint bufLength = 0;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &bufLength );
        if( bufLength )
        {
            char *buf = (char*)malloc( bufLength );
            if( buf )
            {
                glGetProgramInfoLog( program, bufLength, NULL, buf );
                DEBUGLOG( "Could not link program:\n%s\n", buf );
                free( buf );
            }
        }
        glDeleteProgram( program );
        program = 0;
        return false;
    }
    return true;
}



int CCDeviceRenderer::getShaderUniformLocation(const char *name)
{
    return glGetUniformLocation( currentShader->program, name );
}


bool CCDeviceRenderer::loadShader(CCShader *shader)
{
	GLuint vertShader, fragShader;
	
	CCText shaderPath = shader->name;
	shaderPath += ".fx";
	
    // Create shader program
	shader->program = glCreateProgram();
	
	if( !compileShader( &vertShader, GL_VERTEX_SHADER, shaderPath.buffer ) )
	{
		DEBUGLOG( "Failed to compile vertex shader." );
		glDeleteProgram( shader->program );
		shader->program = 0;
		return false;
	}
	
	if( !compileShader( &fragShader, GL_FRAGMENT_SHADER, shaderPath.buffer ) )
	{
		DEBUGLOG( "Failed to compile fragment shader." );
		glDeleteProgram( shader->program );
		shader->program = 0;
		return false;
	}
	
	DEBUG_OPENGL();
	
	// Attach shaders to program
	glAttachShader( shader->program, vertShader );
    DEBUG_OPENGL();
    
	glAttachShader( shader->program, fragShader );
    DEBUG_OPENGL();
	
    // Bind attribute locations - this needs to be done prior to linking
	glBindAttribLocation( shader->program, ATTRIB_VERTEX, "vs_position" );
	glBindAttribLocation( shader->program, ATTRIB_TEXCOORD, "vs_texCoord" );
	glBindAttribLocation( shader->program, ATTRIB_COLOUR, "vs_colour" );
	glBindAttribLocation( shader->program, ATTRIB_NORMAL, "vs_normal" );
    DEBUG_OPENGL();
	
	if( !linkProgram( shader->program ) )
	{
		DEBUGLOG( "Failed to link program." );
		if( vertShader )
		{
			glDeleteShader( vertShader );
			vertShader = 0;
		}
		
		if( fragShader )
		{
			glDeleteShader( fragShader );
			fragShader = 0;
		}
		
		if( shader->program )
		{
			glDeleteProgram( shader->program );
			shader->program = 0;
		}
		
		return false;
	}
    
    // Release vertex and fragment shaders
    if( vertShader )
	{
		glDeleteShader( vertShader );
	}
	
	if( fragShader )
	{
		glDeleteShader( fragShader );
	}
	
    return true;
}


bool CCDeviceRenderer::createDefaultFrameBuffer(CCFrameBufferObject &fbo)
{
    fbo.setFrameBuffer( 0 );
    fbo.width = gView->getWidth();
    fbo.height = gView->getHeight();
    return true;
}


void CCDeviceRenderer::refreshScreenSize()
{
	screenSize.width = gView->getWidth();
	screenSize.height = gView->getHeight();
}


void GLVertexAttribPointer(uint index, int size, GLenum type, bool normalized, int stride, const void *pointer)
{
	glVertexAttribPointer( index, size, type, normalized, stride, pointer );
}


void GLUniform3fv(int location, int count, const GLfloat *value)
{
    glUniform3fv( location, count, value );
}


void GLUniform4fv(int location, int count, const GLfloat *value)
{
    glUniform4fv( location, count, value );
}


void GLUniformMatrix4fv(int location, int count, bool transpose, const GLfloat value[4][4])
{
	glUniformMatrix4fv( location, count, transpose, &value[0][0] );
}
