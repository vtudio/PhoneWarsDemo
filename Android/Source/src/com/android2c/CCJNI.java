/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright � 2010 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCJNI.java
 * Description : Interfaces with ndk library.
 *
 * Created     : 09/10/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

package com.android2c;

import android.util.DisplayMetrics;
import android.util.Log;


// Wrapper for native library
public class CCJNI
{
	static final String library = "ccjni";
	static final boolean debug = false;
	
	static CCActivity activity;
	static String dataPath;
	static String packageName;

    public static void SetActivity(CCActivity inActivity)
    {
    	CCJNI.activity = inActivity;
    	packageName = activity.getPackageName();
    	dataPath = activity.getFilesDir().getAbsolutePath();

    	FileManagerSetPaths( activity.getPackageResourcePath(), dataPath );
    	
    	// Set the screen dpi
    	{
    		DisplayMetrics dm = new DisplayMetrics();
    		activity.getWindowManager().getDefaultDisplay().getMetrics( dm );
    		float xDpi = dm.xdpi;
    		float yDpi = dm.ydpi;
    		ControlsSetDPI( xDpi, yDpi );
    	}
    }
	
	// Load our library
	static 
 	{
 		System.loadLibrary( CCJNI.library );
 		System.out.println();
 	}

	
 	// main
	public static native void OnSurfaceChanged(int width, int height);
	public static native void OnDrawFrame();

	public static void AppPaused()
	{
	}
	public static native void AppResumed();
	
	private static void Assert(final String file, final int line, final String message)
	{
        Log.e( CCJNI.library, String.format( "%s: Assert: %i %s", line, message ) );
	}
	
	// CCGLTexture
	private static int TextureLoad(final String filename, final boolean packaged, final boolean mipmap)
	{
		return CCGLTexture.Load( filename, packaged, mipmap );
	}
	
	private static int TextureGetImageWidth()
	{
		return CCGLTexture.GetImageWidth();
	}
	
	private static int TextureGetImageHeight()
	{
		return CCGLTexture.GetImageHeight();
	}
	
	private static int TextureGetRawWidth()
	{
		return CCGLTexture.GetRawWidth();
	}
	
	private static int TextureGetRawHeight()
	{
		return CCGLTexture.GetRawHeight();
	}
	
	private static int TextureGetAllocatedHeight()
	{
		return CCGLTexture.GetAllocatedHeight();
	}
	
	private static boolean TextureDoesTextureExist(final String filename, final boolean packaged)
	{
		return CCGLTexture.DoesTextureExist( filename, packaged );
	}

	
	// CCDeviceControls
	public static native void ControlsSetDPI(float xDpi, float yDpi);
	
	public static native void ControlsHandleTouch(float x, float y, int actionType, int finger);

	public static native boolean ControlsHandleBackButton();
	
	
	// CCDeviceFileManager
	public static native void FileManagerSetPaths(String apkPath, String dataPath);
}
