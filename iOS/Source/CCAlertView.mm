/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCAlertView.mm
 *-----------------------------------------------------------
 */

#import "CCAlertView.h"
#include "CCDefines.h"
#include "CCAppManager.h"

@implementation CCAlertView


-(id)init
{
    self = [super init];
    if( self != NULL )
    {
        interfaceOrientation = UIInterfaceOrientationPortrait;
        shouldDismiss = true;
        self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    }
    return self;
}

-(void)dealloc 
{	
    [CCAppManager::appManager->viewController unregisterRotateableView:self];
	
    [super dealloc];
}


-(void)show
{
    [super show];
    [self initOrientation];
}


-(void)initOrientation
{
    [self setOrientation:[CCAppManager::appManager->viewController getInterfaceOrientation] duration:0.125f];   
}


static const bool isIPad()
{
    const float iPadAspectRatio = 768.0f / 1024.0f;
    const float aspectRatio = gEngine->renderer->getAspectRatio();
    if( CCDistance( iPadAspectRatio, aspectRatio ) < CC_SMALLFLOAT  )
    {
        return true;
    }
    return false;
}


-(void)setOrientation:(UIInterfaceOrientation)inOrientation duration:(const float)duration
{
    if( inOrientation != interfaceOrientation )
    {
        interfaceOrientation = inOrientation;
        
        [UIView setAnimationBeginsFromCurrentState:true];
        [UIView beginAnimations:@"View Flip" context:nil];
        [UIView setAnimationDuration:duration];
        [UIView setAnimationCurve:UIViewAnimationCurveEaseInOut];
        
        [UIView setAnimationDelegate:self];
        [UIView setAnimationDidStopSelector:@selector( finishedAnimation )];
        
        if( interfaceOrientation == UIInterfaceOrientationLandscapeLeft )
        {
            CGAffineTransform transform = CGAffineTransformMakeRotation( CC_DEGREES_TO_RADIANS( 180.0f ) );
            self.transform = transform;
        }
        else if( interfaceOrientation == UIInterfaceOrientationLandscapeRight )
        {
            CGAffineTransform transform = CGAffineTransformMakeRotation( CC_DEGREES_TO_RADIANS( 0.0f ) );
            self.transform = transform;
        }
        interfaceTransform = self.transform;
        [UIView commitAnimations];
    }
}


-(void)finishedAnimation
{
    // Must keep testing to check if the animation has completed successfully
    // As it gets overridden by Apple deduced pop animations
    CGAffineTransform transform = self.transform;
    if( transform.a != interfaceTransform.a )
    {
        // Problem detected, rerun the target animation.
        UIInterfaceOrientation orientation = interfaceOrientation;
        interfaceOrientation = UIInterfaceOrientationPortrait;
        [self setOrientation:orientation duration:0.125f];
    }
}

-(void)dismissWithClickedButtonIndex:(NSInteger)buttonIndex animated:(BOOL)animated {
    if( shouldDismiss == false )
    {
        return;
    }
    [super dismissWithClickedButtonIndex:buttonIndex animated:animated];
}



@end
