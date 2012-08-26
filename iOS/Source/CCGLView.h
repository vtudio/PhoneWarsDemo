/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCGLView.h
 * Description : OpenGL view.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCGLVIEW_H__
#define __CCGLVIEW_H__


#import "CCDefines.h"
#import "CCViewController.h"

struct ThreadMutex
{
    ThreadMutex();
    ~ThreadMutex();
    
	pthread_mutex_t mutex;
    pthread_mutexattr_t type;
};

@class CCGLView;
extern CCGLView *gView;

@interface CCGLView : UIView<UIAlertViewDelegate>
{	
@public
    ThreadMutex *nativeThreadMutex;
    ThreadMutex *jobsThreadMutex;
	NSTimer *updateTimer;
    
@protected
    CCViewController *viewController;
}

-(void)shutdown;

-(void)setup;
-(void)setViewController:(CCViewController*)controller;

-(void)updateNativeThread;

@end


#endif // __CCGLVIEW_H__
