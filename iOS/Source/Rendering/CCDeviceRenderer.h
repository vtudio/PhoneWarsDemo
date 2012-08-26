/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCDeviceRenderer.h
 * Description : iOS specific OpenGL renderer.
 *
 * Created     : 01/05/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCDEVICERENDERER_H__
#define __CCDEVICERENDERER_H__


#include "CCRenderer.h"

#define ENABLE_MULTISAMPLING 1

class CCDeviceRenderer : public CCRenderer
{
public:
    typedef CCRenderer super;
    
    CCDeviceRenderer();
	virtual ~CCDeviceRenderer();
	
	void clear();
	void resolve();
	
private:
	bool linkProgram(GLuint prog);
    int getShaderUniformLocation(const char *name);
    bool loadShader(CCShader *shader);
    
    virtual bool createContext();
	bool createDefaultFrameBuffer(CCFrameBufferObject &fbo);
    
    void refreshScreenSize();
	
private:
	EAGLContext *context;
    
    bool useMultisampling;
#if ENABLE_MULTISAMPLING
    // Buffer definitions for the MSAA
    GLuint frameBufferMSAA, renderBufferMSAA;
#endif
};


#endif // __CCDEVICERENDERER_H__