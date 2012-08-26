/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 - 2011 SoftPoetry
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCAlertView.h
 * Description : Oriented alert view.
 *
 * Created     : 21/12/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#include "CCViewController.h"

@interface CCAlertView : UIAlertView<CCRotateableView>
{
@protected
    UIInterfaceOrientation interfaceOrientation;
    CGAffineTransform interfaceTransform;
    
@public
    bool shouldDismiss;
}

-(void)setOrientation:(UIInterfaceOrientation)inOrientation duration:(const float)duration;

@end
