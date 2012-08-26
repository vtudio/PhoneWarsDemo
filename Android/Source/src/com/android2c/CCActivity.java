/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCActivity.java
 * Description : Android entry point.
 *
 * Created     : 15/06/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

package com.android2c;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.KeyEvent;
import android.widget.RelativeLayout;


public class CCActivity extends Activity 
{   
	public static boolean isActive = false;
	
	RelativeLayout layout;
    CCGLViewJNI glView = null;
    
    @Override 
    protected void onCreate(Bundle icicle) 
    {
        super.onCreate( icicle );
        this.setRequestedOrientation( ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE );
        
        CCJNI.SetActivity( this );

        glView = new CCGLViewJNI( getApplication() );
        layout = new RelativeLayout( this );
        layout.addView( glView );
        
        setContentView( layout );
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) 
    {
        if( keyCode == KeyEvent.KEYCODE_BACK )
        {
        	if( CCJNI.ControlsHandleBackButton() )
        	{
        		return true;
        	}
        }
        
        return super.onKeyDown( keyCode, event );
    }

    // Called when this activity becomes visible.
    @Override
    protected void onStart() 
    {
        super.onStart();
    }

    @Override
    protected void onResume() 
    {   
        CCJNI.AppResumed();
        super.onResume();
        glView.onResume();
        
        isActive = true;
    }

    @Override
    protected void onPause() 
    {
        CCJNI.AppPaused();
        super.onPause();
        glView.onPause();
        
        isActive = false;
    }

    // Called when this activity is no longer visible.
    @Override
    protected void onStop() 
    {
        super.onStop();
    }
    
    @Override
    public void onDestroy() 
    {
    	super.onDestroy();
    }
}
