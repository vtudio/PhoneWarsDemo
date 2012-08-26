/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCViewController.mm
 *-----------------------------------------------------------
 */

#import "CCViewController.h"
#import "CCGLView.h"
#include "CCAppManager.h"


@implementation CCViewController


-(id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if( self != NULL )
    {
        deviceOrientation = UIDeviceOrientationPortrait;
        interfaceOrientation = UIInterfaceOrientationPortrait;
        orientationAngle = 0.0f;
        orientationUpdateTimer = 0.0f;
        
        rotateableViews = [[NSMutableArray alloc] init];
        
        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
        {
            NSDictionary *info = [NSDictionary 
                                  dictionaryWithContentsOfFile:[NSString stringWithFormat:@"%@/%@", [[NSBundle mainBundle] bundlePath], @"Info.plist"]];
            id launchOrientation = [info objectForKey:@"UIInterfaceOrientation"];
            NSString *interfaceOrientationString = (NSString*)launchOrientation;
            
            if( [interfaceOrientationString compare:@"UIInterfaceOrientationPortraitUpsideDown"] == NSOrderedSame )
            {
                deviceOrientation = UIDeviceOrientationPortraitUpsideDown;
            }
            else if( [interfaceOrientationString compare:@"UIInterfaceOrientationLandscapeLeft"] == NSOrderedSame )
            {
                deviceOrientation = UIDeviceOrientationLandscapeRight;
            }
            else if( [interfaceOrientationString compare:@"UIInterfaceOrientationLandscapeRight"] == NSOrderedSame )
            {
                deviceOrientation = UIDeviceOrientationLandscapeLeft;
            }
        }
    }
    return self;
}


-(void)dealloc 
{	
    [rotateableViews release];
	
    [super dealloc];
}


// Implement loadView to create a view hierarchy programmatically, without using a nib.
-(void)loadView 
{
	// get the window frame here.
	self.view = gView;
}


-(void)viewDidUnload 
{
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = NULL;
}


-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)orientation 
{	
    static bool set = false;
    if( set == false && orientation == UIInterfaceOrientationPortrait )
    {
        set = true;
        return true;
    }
	return false;
}


-(void)toggleAdverts:(const bool)toggle
{
}


static const UIInterfaceOrientation GetInterfaceOrientation(UIDeviceOrientation orientation)
{
	if( orientation == UIDeviceOrientationPortraitUpsideDown )
	{
		return UIInterfaceOrientationPortraitUpsideDown;
	}
	else if( orientation == UIDeviceOrientationLandscapeRight )
	{
		return UIInterfaceOrientationLandscapeLeft;
	}
	else if( orientation == UIDeviceOrientationLandscapeLeft )
	{
		return UIInterfaceOrientationLandscapeRight;
	}
    return UIInterfaceOrientationPortrait;
}


static const float GetOrientationAngle(UIDeviceOrientation orientation)
{
    if( orientation == UIDeviceOrientationPortraitUpsideDown )
    {
        return 180.0f;
    }
    else if( orientation == UIDeviceOrientationLandscapeRight )
	{
        return 90.0f;
	}
	else if( orientation == UIDeviceOrientationLandscapeLeft )
	{
        return 270.0;
	}
    return 0.0f;
}


-(void)detectOrientationUpdate:(const float)delta
{
    orientationUpdateTimer -= delta;
    if( orientationUpdateTimer <= 0.0f )
    {   
        UIDeviceOrientation detectedDeviceOrientation = [[UIDevice currentDevice] orientation];
        if( detectedDeviceOrientation >= UIDeviceOrientationPortrait && detectedDeviceOrientation <= UIDeviceOrientationLandscapeRight )
        {
            if( detectedDeviceOrientation != deviceOrientation )
            {
                const float angle = GetOrientationAngle( detectedDeviceOrientation );
                if( gEngine->isOrientationSupported( angle ) )
                {
                    deviceOrientation = detectedDeviceOrientation;
                    [self refreshOrientation:true];
                }
            }
        }
        orientationUpdateTimer += 0.5f;
    }
}



-(void)setOrientation:(const float)angle
{
    UIDeviceOrientation orientation = UIDeviceOrientationPortrait;
    if( angle == 90.0f )
    {
        orientation = UIDeviceOrientationLandscapeRight;
    }
    else if( angle == 180.0f )
    {
        orientation = UIDeviceOrientationPortraitUpsideDown;
    }
    else if( angle == 270.0f )
    {
        orientation = UIDeviceOrientationLandscapeLeft;
    }
    
    if( orientation != deviceOrientation )
    {
        self->deviceOrientation = orientation;
        [self refreshOrientation:true];
    }
}


-(void)refreshOrientation:(const bool)interpolate
{
	interfaceOrientation = GetInterfaceOrientation( deviceOrientation );
    orientationAngle = GetOrientationAngle( deviceOrientation );
    
	[[UIApplication sharedApplication] setStatusBarOrientation:interfaceOrientation animated:interpolate];
	
    CCAppManager::SetOrientation( orientationAngle, interpolate );
    
    for( uint i=0; i<[rotateableViews count]; ++i )
    {
        id<CCRotateableView> view = [rotateableViews objectAtIndex:i];
        [view setOrientation:interfaceOrientation duration:0.5f];
    }
}


-(UIInterfaceOrientation)getInterfaceOrientation
{
    return interfaceOrientation;
}


-(void)registerRotateableView:(id<CCRotateableView>)view
{
    [rotateableViews addObject:view];
}


-(void)unregisterRotateableView:(id<CCRotateableView>)view
{
    [rotateableViews removeObject:view];
}

@end
