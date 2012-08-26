/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCDeviceRenderer.cpp
 *-----------------------------------------------------------
 */

#import "CCDefines.h"
#import "CCDeviceRenderer.h"
#import "CCGLView.h"
#import "UIDevice-Hardware.h"
#import <QuartzCore/QuartzCore.h>


CCDeviceRenderer::CCDeviceRenderer()
{
    // Disable multisampling on first gen iPads
    useMultisampling = false && [[UIDevice currentDevice] platformType] != UIDevice1GiPad;

    // Disable multisampling on non-iOS4 devices
    #define kCFCoreFoundationVersionNumber_iPhoneOS_4_0 550.32
    if( kCFCoreFoundationVersionNumber < kCFCoreFoundationVersionNumber_iPhoneOS_4_0 )
    {
        useMultisampling = false;
    }
    
    usingOpenGL2 = true;
}

CCDeviceRenderer::~CCDeviceRenderer()
{
    frameBufferManager.destoryAllFrameBuffers();
    
#if ENABLE_MULTISAMPLING
    
    if( useMultisampling )
    {
        glDeleteFramebuffers( 1, &frameBufferMSAA );
        glDeleteRenderbuffers( 1, &renderBufferMSAA );
    }
    
#endif
    
    if( [EAGLContext currentContext] == context ) 
	{
		[EAGLContext setCurrentContext:NULL];
    }
    [context release];
}


void CCDeviceRenderer::clear()
{	
#if ENABLE_MULTISAMPLING
    
    if( useMultisampling )
    {
        glBindFramebuffer( GL_FRAMEBUFFER, frameBufferMSAA );
    }
    else
    {
        glBindFramebuffer( GL_FRAMEBUFFER, getDefaultFrameBuffer() );
    }
    
#else
    
    glBindFramebuffer( GL_FRAMEBUFFER, frameBuffer );
    
#endif
    
	super::clear();
}


void CCDeviceRenderer::resolve()
{	
#if ENABLE_MULTISAMPLING
    
    if( useMultisampling )
    {
        // Apple (and the khronos group) encourages you to discard depth 
        // render buffer contents whenever is possible 
        static GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT };
        glDiscardFramebufferEXT( GL_READ_FRAMEBUFFER_APPLE, 2, attachments );
        
        glBindFramebuffer( GL_READ_FRAMEBUFFER_APPLE, frameBufferMSAA );
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER_APPLE, getDefaultFrameBuffer() );
        glResolveMultisampleFramebufferAPPLE();
    }
    
#endif
    
    glBindRenderbuffer( GL_RENDERBUFFER, getDefaultFrameBuffer() );
    [context presentRenderbuffer:GL_RENDERBUFFER];
}


static const bool compileShader(GLuint *shader, GLenum type, NSString *file)
{
    GLint status;
	
    NSString *sourceCode = [NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil];
    if( !sourceCode )
    {
        NSLog( @"Failed to load vertex shader" );
        return false;
    }
    
    NSString *appendedCode = NULL;
    if( type == GL_VERTEX_SHADER )
    {
        const NSString *defines = @"#define VERTEX_SHADER\r\n";
        appendedCode = [NSString stringWithFormat:@"%@%@", defines, sourceCode];
    }
    else if( type == GL_FRAGMENT_SHADER )
    {
        const NSString *defines = @"#define PIXEL_SHADER\r\n";
        appendedCode = [NSString stringWithFormat:@"%@%@", defines, sourceCode];
    }
    const GLchar *source = [appendedCode UTF8String];
    
    *shader = glCreateShader( type );
    glShaderSource( *shader, 1, &source, NULL );
    glCompileShader( *shader );
	
#if DEBUGON
    GLchar *log = NULL;
    GLint logLength;
    glGetShaderiv( *shader, GL_INFO_LOG_LENGTH, &logLength );
    if( logLength > 0 )
    {
        log = (GLchar*)malloc( logLength );
        glGetShaderInfoLog( *shader, logLength, &logLength, log );
        NSLog( @"Shader compile log:\n%s", log );
    }
#endif
	
    glGetShaderiv( *shader, GL_COMPILE_STATUS, &status );
    bool result = true;
    if( status == 0 )
    {
        glDeleteShader( *shader );
        ASSERT( false );
        result = false;
    }
    
#if DEBUGON
    free( log );
#endif
	
    return result;
}


bool CCDeviceRenderer::linkProgram(GLuint prog)
{
    GLint status;
	
    glLinkProgram( prog );
	
#if DEBUGON
    GLint logLength;
    glGetProgramiv( prog, GL_INFO_LOG_LENGTH, &logLength );
    if (logLength > 0)
    {
        GLchar *log = (GLchar*)malloc( logLength );
        glGetProgramInfoLog( prog, logLength, &logLength, log );
        NSLog( @"Program link log:\n%s", log );
        free( log);
    }
#endif
	
    glGetProgramiv( prog, GL_LINK_STATUS, &status );
    if( status == 0 )
    {
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
    NSString *shaderPath = [[NSBundle mainBundle] pathForResource:[NSString stringWithFormat:@"%s", shader->name] ofType:@"fx"];
	
    // Create shader program
    shader->program = glCreateProgram();
	
    // Create and compile vertex shader
    if( !compileShader( &vertShader, GL_VERTEX_SHADER, shaderPath ) )
    {
        NSLog( @"Failed to compile vertex shader" );
        return false;
    }
	
    // Create and compile fragment shader
    if( !compileShader( &fragShader, GL_FRAGMENT_SHADER, shaderPath ) )
    {
        ASSERT( false );
        NSLog( @"Failed to compile fragment shader" );
        return false;
    }
	
    // Attach vertex shader to program
    glAttachShader( shader->program, vertShader );
	
    // Attach fragment shader to program
    glAttachShader( shader->program, fragShader );
	
    // Bind attribute locations
    // this needs to be done prior to linking
    glBindAttribLocation( shader->program, ATTRIB_VERTEX, "vs_position" );
	glBindAttribLocation( shader->program, ATTRIB_TEXCOORD, "vs_texCoord" );
    glBindAttribLocation( shader->program, ATTRIB_COLOUR, "vs_colour" );
    glBindAttribLocation( shader->program, ATTRIB_NORMAL, "vs_normal" );
	
    // Link program
    if( !linkProgram( shader->program ) )
    {
        NSLog( @"Failed to link program: %d", shader->program );
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


bool CCDeviceRenderer::createContext()
{
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if( !context || ![EAGLContext setCurrentContext:context] )
    {
        usingOpenGL2 = false;
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        if( !context || ![EAGLContext setCurrentContext:context] )
        {
            return false;
        }
    }
    return true;
}


bool CCDeviceRenderer::createDefaultFrameBuffer(CCFrameBufferObject &fbo)
{
    GLuint frameBuffer = 0;
    GLuint renderBuffer = 0;
    GLuint depthBuffer = 0;
    int backBufferWidth, backBufferHeight;
    
	// Create default buffers
	glGenFramebuffers( 1, &frameBuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, frameBuffer );
    
	glGenRenderbuffers( 1, &renderBuffer );
	glBindRenderbuffer( GL_RENDERBUFFER, renderBuffer );
	
	// Allocate color buffer backing based on the current layer size
	[context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)gView.layer];
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer );
    
	glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backBufferWidth );
	glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backBufferHeight );

#if ENABLE_MULTISAMPLING
    
    if( usingOpenGL2 && useMultisampling )
    {
        // Create msaa buffers
        glGenFramebuffers( 1, &frameBufferMSAA );
        glBindFramebuffer( GL_FRAMEBUFFER, frameBufferMSAA );
        
        glGenRenderbuffers( 1, &renderBufferMSAA );
        glBindRenderbuffer( GL_RENDERBUFFER, renderBufferMSAA );
        
        // Generate the msaaDepthBuffer. 
        // 4 will be the number of pixels that the MSAA buffer will use in order to make one pixel on the render buffer. 
        glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER, 4, GL_RGBA8_OES, backBufferWidth, backBufferHeight );
        //glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER, 4, GL_RGB5_A1, backBufferWidth, backBufferHeight ); 
        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBufferMSAA ); 
         
        // msaa depth buffer
        glGenRenderbuffers( 1, &depthBuffer );
        glBindRenderbuffer( GL_RENDERBUFFER, depthBuffer );
        glRenderbufferStorageMultisampleAPPLE( GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT16, backBufferWidth, backBufferHeight );
        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    }
    else
    {
        glGenRenderbuffers( 1, &depthBuffer );
        glBindRenderbuffer( GL_RENDERBUFFER, depthBuffer );
        
        glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backBufferWidth, backBufferHeight );
        glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer );
    }
    
#else
	
    glGenRenderbuffers( 1, &depthBuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, depthBuffer );
    
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backBufferWidth, backBufferHeight );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer );
    
#endif
	
	if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
	{
		NSLog( @"Failed to make complete framebuffer object %x", glCheckFramebufferStatus( GL_FRAMEBUFFER ) );
		ASSERT( false );
		return false;
	}
    
    fbo.setFrameBuffer( frameBuffer );
    fbo.renderBuffer = renderBuffer;
    fbo.depthBuffer = depthBuffer;
    fbo.width = backBufferWidth;
    fbo.height = backBufferHeight;
	
    return true;
}


void CCDeviceRenderer::refreshScreenSize()
{
    screenSize.width = gView.bounds.size.width;
    screenSize.height = gView.bounds.size.height;
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