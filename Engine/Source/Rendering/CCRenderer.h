/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCRenderer.h
 * Description : Common OpenGL renderer.
 *
 * Created     : 01/05/10
 * Author(s)   : Ashraf Samy Hegab, Chris Wilson
 *-----------------------------------------------------------
 */

#ifndef __CCRENDERER_H__
#define __CCRENDERER_H__


#include "CCFrameBufferManager.h"


enum CCRenderFlags
{
    render_all				= 0x000000001,
    render_collisionBoxes	= 0x000000002,
    render_collisionTrees	= 0x000000004,
    render_pathFinder		= 0x000000008,
    render_noPyramids		= 0x000000010,
    render_fontPage         = 0x000000012,
};

enum CCRenderPass
{
	render_background,
	render_main,
	render_finished
};

// uniform index
enum 
{
	UNIFORM_MODELVIEWPROJECTIONMATRIX,
    UNIFORM_MODELVIEWMATRIX,
    UNIFORM_MODELCOLOUR,
    
    UNIFORM_MODELNORMALMATRIX,
    UNIFORM_LIGHTPOSITION,
    UNIFORM_LIGHTDIFFUSE,
    
    UNIFORM_CAMERAPOSITION,
    
    TEXTURE_DIFFUSE,
    TEXTURE_ENV,
    
    NUM_UNIFORMS
};

// attribute index
enum 
{
    ATTRIB_VERTEX,
	ATTRIB_TEXCOORD,
    ATTRIB_COLOUR,
    ATTRIB_NORMAL,
    NUM_ATTRIBUTES
};

struct CCShader
{
    CCShader(const char *name);
    
    void use();
    void enableAttributeArray(const uint index);
    void disableAttributeArray(const uint index);
    
    const char *name;
    GLint uniforms[NUM_UNIFORMS];
    
#ifdef QT
    class QGLShaderProgram *program;
#else
    GLuint program;
#endif
};


class CCRenderer
{
    friend class CCFrameBufferManager;
    
public:
    virtual ~CCRenderer();

    bool setup(const bool clear);
    
    bool openGL2() { return usingOpenGL2; }

    // Calculates the screen size parameters
    void setupScreenSizeParams();

    virtual void clear();
    virtual void resolve() {};

protected:
    virtual int getShaderUniformLocation(const char *name) = 0;
    CCShader* loadShader(const char *name);
    virtual bool loadShader(CCShader *shader) = 0;
    virtual bool loadShaders();

    virtual bool createContext() { return true; }
    virtual bool createDefaultFrameBuffer(CCFrameBufferObject &fbo) = 0;

    // Gets the current screen size from the view
    virtual void refreshScreenSize() = 0;

public:
    void setupOpenGL();
    
    inline FBOType getDefaultFrameBuffer() { return frameBufferManager.defaultFBO.getFrameBuffer(); }
    
    inline const CCShader* getShader() { return currentShader; }
    bool setShader(const char *name, const bool useVertexColours=false, const bool useVertexNormals=false);
    
    const CCSize& getScreenSize() { return screenSize; }
    const CCSize& getInverseScreenSize() { return inverseScreenSize; }
    float getAspectRatio() { return aspectRatio; }
    
protected:
    CCSize screenSize;
    CCSize inverseScreenSize;
    float aspectRatio;

    bool clearScreenRequired;
    
public:
    CCFrameBufferManager frameBufferManager;
    
protected:
    CCShader *currentShader;
    CCList<CCShader> shaders;
    
    bool usingOpenGL2;
};

extern void GLUseProjectionMatrix(const bool toggle);
extern void CCSetViewMatrix();

extern void GLEnableBlend();
extern void GLDisableBlend();
extern bool GLBlendState();
extern void GLEnableDepth();
extern void GLDisableDepth();
extern bool GLDepthState();

// Attempt to simulate OpenGL 1.1 interface to shaders
extern void GLVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
extern void GLTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
extern void GLColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

extern void CCSetVertexAttribute(const uint attribute, 
                               GLint size, GLenum type, GLsizei stride,
                               const GLvoid *pointer, const bool normalized);
extern void CCSetUniformVector3(const uint uniform, 
                                const float x, const float y, const float z);
extern void CCSetUniformVector4(const uint uniform, 
                                const float x, const float y, const float z, const float w);

// Implemented in DeviceRenderer, as they require device specific hooks
extern void GLVertexAttribPointer(uint index, int size, GLenum type, bool normalized, int stride, const void *pointer);
extern void GLUniform3fv(int location, int count, const GLfloat *value); 
extern void GLUniform4fv(int location, int count, const GLfloat *value); 
extern void GLUniformMatrix4fv(int location, int count, bool transpose, const GLfloat value[4][4]);

#endif // __CCRENDERER_H__
