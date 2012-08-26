/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCViewController.h
 * Description : iOS view controller for handling orientation and views.
 *
 * Created     : 07/06/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

@protocol CCRotateableView<NSObject>

-(void)setOrientation:(UIInterfaceOrientation)interfaceOrientation duration:(const float)duration;

@end

@interface CCViewController : UIViewController
{
@protected
    UIDeviceOrientation deviceOrientation;
    UIInterfaceOrientation interfaceOrientation;
    float orientationAngle;
	float orientationUpdateTimer;
    
    NSMutableArray *rotateableViews;
}

-(void)toggleAdverts:(const bool)toggle;

-(void)detectOrientationUpdate:(const float)delta;

-(void)setOrientation:(const float)angle;
-(void)refreshOrientation:(const bool)instant;
-(UIInterfaceOrientation)getInterfaceOrientation;

-(void)registerRotateableView:(id<CCRotateableView>)view;
-(void)unregisterRotateableView:(id<CCRotateableView>)view;

@end
