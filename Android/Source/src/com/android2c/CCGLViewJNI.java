/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCGLViewJNI.java
 * Description : Creates the OpenGL view.
 * 				 Based off hello-gl2 sample provided in AndroidNDKr5b
 * 				 http://developer.android.com/sdk/ndk/
 *
 * Created     : 09/06/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

package com.android2c;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.opengles.GL11;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;


class CCGLViewJNI extends GLSurfaceView
{	
	public static GL11 glContext;
    static boolean resuming = false;
    static int width, height = 0;
	
    public CCGLViewJNI(Context context) 
    {
        super( context );
        
        // By default GLSurfaceView will create a PixelFormat.RGB_565 format surface.
        // If a translucent surface is required, call getHolder().setFormat(PixelFormat.TRANSLUCENT). 
        // The exact format of a TRANSLUCENT surface is device dependent, but it will be a 32-bit-per-pixel surface with 8 bits per component. 
        this.getHolder().setFormat( PixelFormat.TRANSLUCENT );
        setEGLConfigChooser( new ConfigChooser( 8, 8, 8, 8, 16, 0 ) );
        //setEGLConfigChooser( new ConfigChooser( 5, 6, 5, 0, 16, 0 ) );

        // Create OpenGL context
        //setEGLContextFactory( new ContextFactory() );
        setEGLContextClientVersion( 2 );

        // Set our renderer
        setRenderer( new Renderer() );
    }
    
    public void onPause()
    {
    	super.onPause();
    }
    
    public void onResume()
    {
    	resuming = true;
    	super.onResume();
    }

    boolean[] touches = new boolean[2];
    float[] xs = new float[2];
    float[] ys = new float[2];
    public boolean onTouchEvent(final MotionEvent event) 
    {
    	final int action = event.getAction() & MotionEvent.ACTION_MASK;;
    	final int pointerIndex = ( event.getAction() & MotionEvent.ACTION_POINTER_INDEX_MASK ) >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
    	final int touchId = event.getPointerId( pointerIndex );
    	
    	if( touchId >= 0 && touchId < 2 )
    	{
    		if( touchId >= 1 )
    		{
    			final int pointerCount = event.getPointerCount();
    			if( pointerCount < 2 )
    			{
    				return true;
    			}
    		}
	    	// Release touches
	    	switch( action )
	    	{
	        case MotionEvent.ACTION_UP:
	        case MotionEvent.ACTION_CANCEL:
	        case MotionEvent.ACTION_OUTSIDE:
	        case MotionEvent.ACTION_POINTER_UP: 
		        {
	        		if( touches[touchId] )
	        		{
	        			touches[touchId] = false;

	        			try
		    			{
		    		    	final float x = event.getX( touchId );
		    		    	final float y = event.getY( touchId );
		    		    	xs[touchId] = x;
		    		    	ys[touchId] = y;
		    	    		CCJNI.ControlsHandleTouch( x, y, action, touchId );
		    			}
	        			catch( Exception e )
	        			{
		    	    		CCJNI.ControlsHandleTouch( xs[touchId], ys[touchId], action, touchId );
	        			}
	    				return true;
	        		}
		        }
		        break;

	        case MotionEvent.ACTION_DOWN:
	        case MotionEvent.ACTION_POINTER_DOWN:
	        	{
	        		// Register any unregistered touches
	        		if( touches[touchId] == false )
	        		{
	        			touches[touchId] = true;

	    		    	final float x = event.getX( touchId );
	    		    	final float y = event.getY( touchId );
	    		    	xs[touchId] = x;
	    		    	ys[touchId] = y;
	    				CCJNI.ControlsHandleTouch( x, y, action, touchId );
	    				return true;
	        		}
	        	}
	        	break;
	        	
	        case MotionEvent.ACTION_MOVE:
		        {
		        	// Update moving touches
			    	for( int i=0; i<2; ++i )
			    	{
			    		if( touches[i] )
			    		{
			    			try
			    			{
			    				final float x = event.getX( i );
			    				final float y = event.getY( i );
			    		    	xs[i] = x;
			    		    	ys[i] = y;
			    				CCJNI.ControlsHandleTouch( x, y, action, i );
			    			}
			    			catch( Exception e ) 
			    	        {
			    	            // Ignore.
			    	        	e.printStackTrace();
			    	        }
			    		}
			    	}
		        }
		        break;
	        }
    	}
    	else if( CCJNI.debug )
    	{
    		System.out.println();
    	}
        return true;
    }
    

    private static class Renderer implements GLSurfaceView.Renderer 
    {
        public void onDrawFrame(GL10 gl) 
        {
        	glContext = (GL11)gl;
            CCJNI.OnDrawFrame();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) 
        {
        	if( resuming || CCGLViewJNI.width != width || CCGLViewJNI.height != height )
        	{
        		//resuming = false; // The screen doesn't resize properly on some devices, so always reset for now
        		
        		CCGLViewJNI.width = width;
        		CCGLViewJNI.height = height;
            	glContext = (GL11)gl;
        		CCJNI.OnSurfaceChanged( width, height );
        	}
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) 
        {
            // Do nothing.
        }
    }
    
    
    static void checkGLError(String prompt, GL10 gl)
    {
        int error;
        while( ( error = gl.glGetError() ) != 0 ) 
        {
            Log.e( CCJNI.library, String.format( "%s: EGL error: 0x%x", prompt, error ) );
        }
    }
    
    
    static void checkEglError(String prompt, EGL10 egl)
    {
        int error;
        while( ( error = egl.eglGetError() ) != EGL10.EGL_SUCCESS ) 
        {
            Log.e( CCJNI.library, String.format( "%s: EGL error: 0x%x", prompt, error ) );
        }
    }
    
    
    private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser 
    {
        public ConfigChooser(int r, int g, int b, int a, int depth, int stencil) 
        {
            mRedSize = r;
            mGreenSize = g;
            mBlueSize = b;
            mAlphaSize = a;
            mDepthSize = depth;
            mStencilSize = stencil;
        }

        /* This EGL config specification is used to specify 2.0 rendering.
         * We use a minimum size of 4 bits for red/green/blue, but will
         * perform actual matching in chooseConfig() below.
         */
        private static int EGL_OPENGL_ES2_BIT = 4;
        private static int[] s_configAttribs2 =
        {
            EGL10.EGL_RED_SIZE, 4,
            EGL10.EGL_GREEN_SIZE, 4,
            EGL10.EGL_BLUE_SIZE, 4,
            EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL10.EGL_NONE
        };

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) 
        {
            /* Get the number of minimally matching EGL configurations
             */
            int[] num_config = new int[1];
            egl.eglChooseConfig( display, s_configAttribs2, null, 0, num_config );

            int numConfigs = num_config[0];

            if( numConfigs <= 0 ) 
            {
                throw new IllegalArgumentException( "No configs match configSpec" );
            }

            /* Allocate then read the array of minimally matching EGL configs
             */
            EGLConfig[] configs = new EGLConfig[numConfigs];
            egl.eglChooseConfig( display, s_configAttribs2, configs, numConfigs, num_config );

            if( CCJNI.debug ) 
            {
                 printConfigs( egl, display, configs );
            }
            /* Now return the "best" one
             */
            return chooseConfig( egl, display, configs );
        }

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs) 
        {
            for( EGLConfig config : configs ) 
            {
                int d = findConfigAttrib( egl, display, config, EGL10.EGL_DEPTH_SIZE, 0 );
                int s = findConfigAttrib( egl, display, config, EGL10.EGL_STENCIL_SIZE, 0 );

                // We need at least mDepthSize and mStencilSize bits
                if( d < mDepthSize || s < mStencilSize )
                    continue;

                // We want an *exact* match for red/green/blue/alpha
                int r = findConfigAttrib( egl, display, config, EGL10.EGL_RED_SIZE, 0 );
                int g = findConfigAttrib( egl, display, config, EGL10.EGL_GREEN_SIZE, 0 );
                int b = findConfigAttrib( egl, display, config, EGL10.EGL_BLUE_SIZE, 0 );
                int a = findConfigAttrib( egl, display, config, EGL10.EGL_ALPHA_SIZE, 0 );

                if( r  == mRedSize && g == mGreenSize && b == mBlueSize && a == mAlphaSize )
                    return config;
            }
            return null;
        }

        private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config, int attribute, int defaultValue) 
        {
            if( egl.eglGetConfigAttrib( display, config, attribute, mValue ) ) 
            {
                return mValue[0];
            }
            return defaultValue;
        }

        private void printConfigs(EGL10 egl, EGLDisplay display, EGLConfig[] configs) 
        {
            int numConfigs = configs.length;
            Log.w( CCJNI.library, String.format( "%d configurations", numConfigs ) );
            for (int i = 0; i < numConfigs; i++)
            {
                Log.w( CCJNI.library, String.format("Configuration %d:\n", i) );
                printConfig(egl, display, configs[i]);
            }
        }

        private void printConfig(EGL10 egl, EGLDisplay display, EGLConfig config)
        {
            int[] attributes =
            {
                    EGL10.EGL_BUFFER_SIZE,
                    EGL10.EGL_ALPHA_SIZE,
                    EGL10.EGL_BLUE_SIZE,
                    EGL10.EGL_GREEN_SIZE,
                    EGL10.EGL_RED_SIZE,
                    EGL10.EGL_DEPTH_SIZE,
                    EGL10.EGL_STENCIL_SIZE,
                    EGL10.EGL_CONFIG_CAVEAT,
                    EGL10.EGL_CONFIG_ID,
                    EGL10.EGL_LEVEL,
                    EGL10.EGL_MAX_PBUFFER_HEIGHT,
                    EGL10.EGL_MAX_PBUFFER_PIXELS,
                    EGL10.EGL_MAX_PBUFFER_WIDTH,
                    EGL10.EGL_NATIVE_RENDERABLE,
                    EGL10.EGL_NATIVE_VISUAL_ID,
                    EGL10.EGL_NATIVE_VISUAL_TYPE,
                    0x3030, // EGL10.EGL_PRESERVED_RESOURCES,
                    EGL10.EGL_SAMPLES,
                    EGL10.EGL_SAMPLE_BUFFERS,
                    EGL10.EGL_SURFACE_TYPE,
                    EGL10.EGL_TRANSPARENT_TYPE,
                    EGL10.EGL_TRANSPARENT_RED_VALUE,
                    EGL10.EGL_TRANSPARENT_GREEN_VALUE,
                    EGL10.EGL_TRANSPARENT_BLUE_VALUE,
                    0x3039, // EGL10.EGL_BIND_TO_TEXTURE_RGB,
                    0x303A, // EGL10.EGL_BIND_TO_TEXTURE_RGBA,
                    0x303B, // EGL10.EGL_MIN_SWAP_INTERVAL,
                    0x303C, // EGL10.EGL_MAX_SWAP_INTERVAL,
                    EGL10.EGL_LUMINANCE_SIZE,
                    EGL10.EGL_ALPHA_MASK_SIZE,
                    EGL10.EGL_COLOR_BUFFER_TYPE,
                    EGL10.EGL_RENDERABLE_TYPE,
                    0x3042 // EGL10.EGL_CONFORMANT
            };
            String[] names =
            {
                    "EGL_BUFFER_SIZE",
                    "EGL_ALPHA_SIZE",
                    "EGL_BLUE_SIZE",
                    "EGL_GREEN_SIZE",
                    "EGL_RED_SIZE",
                    "EGL_DEPTH_SIZE",
                    "EGL_STENCIL_SIZE",
                    "EGL_CONFIG_CAVEAT",
                    "EGL_CONFIG_ID",
                    "EGL_LEVEL",
                    "EGL_MAX_PBUFFER_HEIGHT",
                    "EGL_MAX_PBUFFER_PIXELS",
                    "EGL_MAX_PBUFFER_WIDTH",
                    "EGL_NATIVE_RENDERABLE",
                    "EGL_NATIVE_VISUAL_ID",
                    "EGL_NATIVE_VISUAL_TYPE",
                    "EGL_PRESERVED_RESOURCES",
                    "EGL_SAMPLES",
                    "EGL_SAMPLE_BUFFERS",
                    "EGL_SURFACE_TYPE",
                    "EGL_TRANSPARENT_TYPE",
                    "EGL_TRANSPARENT_RED_VALUE",
                    "EGL_TRANSPARENT_GREEN_VALUE",
                    "EGL_TRANSPARENT_BLUE_VALUE",
                    "EGL_BIND_TO_TEXTURE_RGB",
                    "EGL_BIND_TO_TEXTURE_RGBA",
                    "EGL_MIN_SWAP_INTERVAL",
                    "EGL_MAX_SWAP_INTERVAL",
                    "EGL_LUMINANCE_SIZE",
                    "EGL_ALPHA_MASK_SIZE",
                    "EGL_COLOR_BUFFER_TYPE",
                    "EGL_RENDERABLE_TYPE",
                    "EGL_CONFORMANT"
            };
            int[] value = new int[1];
            for (int i = 0; i < attributes.length; i++)
            {
                int attribute = attributes[i];
                String name = names[i];
                if( egl.eglGetConfigAttrib(display, config, attribute, value))
                {
                    Log.w( CCJNI.library, String.format( "  %s: %d\n", name, value[0] ) );
                }
                else
                {
                    // Log.w(TAG, String.format("  %s: failed\n", name));
                    while( egl.eglGetError() != EGL10.EGL_SUCCESS );
                }
            }
        }

        // Subclasses can adjust these values:
        protected int mRedSize;
        protected int mGreenSize;
        protected int mBlueSize;
        protected int mAlphaSize;
        protected int mDepthSize;
        protected int mStencilSize;
        private int[] mValue = new int[1];
    }
}
