/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCGLTexture.java
 * Description : Interfaces with Bitmap class to load textures.
 *
 * Created     : 09/10/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

package com.android2c;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.opengles.GL11;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLUtils;
import android.util.Log;

public class CCGLTexture
{	
	// On Android we always scaled our images to be square sizes
	static int imageWidth = 0;
	static int imageHeight = 0;
	
	// The raw image sizes before being skewed to fit as square textures
	static int rawWidth = 0;
	static int rawHeight = 0;
	
	// The allocated height size
	static int allocatedHeight = 0;
	
	static int NextPowerOfTwo(int x)
	{
		x = x - 1;
		x = x | ( x >> 1 );
		x = x | ( x >> 2 );
		x = x | ( x >> 4 );
		x = x | ( x >> 8 );
		x = x | ( x >>16 );
		return x + 1;
	}
	
	public static boolean DoesTextureExist(final String filename, final boolean packaged)
	{
		boolean exists = false;
		if( packaged )
		{
			int dotIndex = filename.lastIndexOf( '.' );
			final String pureFilename = ( dotIndex > 0 ) ? filename.substring( 0, dotIndex ) : filename;  
			int id = CCJNI.activity.getResources().getIdentifier( pureFilename, "drawable", CCJNI.packageName );
	        if( id > 0 )
	        {
	        	exists = true;
	        }
		}
		
		// Cached
		else
		{
			final String filePath = CCJNI.dataPath + "/" + filename;
			final File file = new File( filePath );
			if( file.exists() )
			{
				exists = true;
			}
		}
		
		return exists;
	}
	
	public static int Load(final String filename, final boolean packaged, final boolean mipmap)
	{
		Bitmap bitmap = null;
		
		if( packaged )
		{
			int dotIndex = filename.lastIndexOf( '.' );
			
			final String pureFilename = ( dotIndex > 0 ) ? filename.substring( 0, dotIndex ) : filename;  
			int id = CCJNI.activity.getResources().getIdentifier( pureFilename, "drawable", CCJNI.packageName );
	
	        try 
	        {
	        	InputStream is = CCJNI.activity.getResources().openRawResource( id );
	            bitmap = BitmapFactory.decodeStream( is, null, null );
	    		is.close();
	        } 
	        catch( Exception e ) 
	        {
	            // Ignore.
	        	e.printStackTrace();
	        }
		}
		
		// Cached
		else
		{
			final String filePath = CCJNI.dataPath + "/" + filename;
			
			try 
	        {
				FileInputStream is = new FileInputStream( filePath );
	            bitmap = BitmapFactory.decodeStream( is );
	    		is.close();
	        } 
	        catch( Exception e ) 
	        {
	            // Ignore.
	        	e.printStackTrace();
	        }
		}
        
        if( bitmap == null )
        {
        	return 0;
        }
        else
        {
        	// Re-scale the width to be a power of 2 to avoid any weird stretching issues.
        	imageWidth = rawWidth = bitmap.getWidth();
        	allocatedHeight = imageHeight = rawHeight = bitmap.getHeight();
        	int widthSquared = NextPowerOfTwo( imageWidth );
        	int heightSquared = NextPowerOfTwo( imageHeight );
        	
        	// Always re-scale as, a few devices have trouble using the provided textures
        	//if( width != widthSquared || height != heightSquared )
        	{
        		float scale = (float)widthSquared/(float)rawWidth;
        		imageWidth = widthSquared;
        		imageHeight = (int)( rawHeight * scale );
        		allocatedHeight = NextPowerOfTwo( imageHeight );
        		
    			Bitmap scaledBitmap = Bitmap.createScaledBitmap( bitmap, widthSquared, heightSquared, true );
    			if( scaledBitmap != bitmap )
    			{
    				bitmap.recycle();
    				bitmap = scaledBitmap;
    			}
        	}
        	
			int[] glName = new int[1];
			
			//GL11 gl = CCGLViewJNI.glContext;
			GL11 gl = CCGLViewJNI.glContext;
	        gl.glGenTextures( 1, glName, 0 );
	
	        gl.glBindTexture( GL10.GL_TEXTURE_2D, glName[0] );

	       	gl.glTexParameterf( GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR );
//	        if( false && mipmap )
//	        {
//	        	gl.glTexParameteri( GL10.GL_TEXTURE_2D, GL11.GL_TEXTURE_MIN_FILTER, GL11.GL_LINEAR_MIPMAP_LINEAR );
//		        gl.glTexParameteri( GL10.GL_TEXTURE_2D, GL11.GL_GENERATE_MIPMAP, GL11.GL_TRUE );
//	        }
//	        else
	        {
	        	gl.glTexParameterf( GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_LINEAR );
	        }
	        
	        Log.i( "CCGLTexture", filename );
	        GLUtils.texImage2D( GL11.GL_TEXTURE_2D, 0, bitmap, 0 );

	    	bitmap.recycle();
	    	bitmap = null;

			// Try to run the garbage collector after a recycle?
			//System.gc();
	        
	        gl.glBindTexture( GL10.GL_TEXTURE_2D, 0 );
	        
	        //CCGLViewJNI.checkGLError( "Load Texture", gl );
	        return glName[0];
        }
	}
	
	public static int GetImageWidth()
	{
		return imageWidth;
	}
	
	public static int GetImageHeight()
	{
		return imageHeight;
	}
	
	public static int GetRawWidth()
	{
		return rawWidth;
	}
	
	public static int GetRawHeight()
	{
		return rawHeight;
	}
	
	public static int GetAllocatedHeight()
	{
		return allocatedHeight;
	}
}
