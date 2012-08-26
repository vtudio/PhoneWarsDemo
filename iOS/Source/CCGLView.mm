/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCGLView.mm
 *-----------------------------------------------------------
 */

#import "CCDefines.h"
#import "CCDeviceControls.h"
#include "CCAppManager.h"

#include "CCAlertView.h"

#import <QuartzCore/QuartzCore.h>
#import <pthread.h>

#define NATIVETHREAD_INTERVAL 0.25f


ThreadMutex::ThreadMutex()
{
    pthread_mutexattr_init( &type );
    pthread_mutexattr_settype( &type, PTHREAD_MUTEX_RECURSIVE );
    pthread_mutex_init( &mutex, &type );
}


ThreadMutex::~ThreadMutex()
{
    pthread_mutex_destroy( &mutex );
    pthread_mutexattr_destroy( &type );
}

@implementation CCGLView

// You must implement this method
+(Class)layerClass 
{
    return [CAEAGLLayer class];
}


-(id)initWithFrame:(CGRect)frame
{	
	self = [super initWithFrame:frame];
    if( self != NULL )
	{
		gView = self;
		
		self.userInteractionEnabled = false;
		self.hidden = false;
        
        nativeThreadMutex = new ThreadMutex();
        jobsThreadMutex = new ThreadMutex();
		
		// Get the layer
		CAEAGLLayer *eaglLayer = (CAEAGLLayer*)self.layer;
        
        // Set contentScale Factor to 2, to use high resolution
        if( [[UIScreen mainScreen] respondsToSelector:@selector( scale )] && [[UIScreen mainScreen] scale] == 2.0 ) 
        {
            self.contentScaleFactor = 2.0f;
            eaglLayer.contentsScale = 2.0f;
        }
        
		eaglLayer.opaque = true;
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSNumber numberWithBool:false], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, NULL];
        
		updateTimer = [NSTimer scheduledTimerWithTimeInterval:0.01f target:self selector:@selector( setup ) userInfo:NULL repeats:false];
	}
	
    return self;
}


-(void)setViewController:(CCViewController*)controller
{
    viewController = controller;
}


-(void)dealloc 
{	
    ASSERT( gEngine == NULL );
	delete nativeThreadMutex;
	delete jobsThreadMutex;
	
    [super dealloc];
}


-(void)shutdown
{	
	self.userInteractionEnabled = false;
	delete gEngine;
	gEngine = NULL;
    gView = NULL;
    
	[self release];
}


static inline void refreshReleasePool(NSAutoreleasePool **pool, uint *count, const uint target)
{
	if( (*count)++ > target )
	{
		[*pool release];
		*pool = [[NSAutoreleasePool alloc] init];
		*count = 0;
	}
}


static bool setupEngineThread = false;

void* PosixEngineThread(void* data)
{	
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	setupEngineThread = gEngine->setupEngineThread();
	[pool release];
    
    if( setupEngineThread == false )
    {
        LAMBDA_UNSAFE( AlertCallback,
            CCAlertView *alertView = [[[CCAlertView alloc] initWithTitle:@"Congratulations"
                                                                 message:@"You have been selected to use the web version of iGrapher."
                                                                delegate:gView
                                                       cancelButtonTitle:@"Ok"
                                                       otherButtonTitles:nil] autorelease];
            alertView->shouldDismiss = false;
            [alertView show];
            [alertView setOrientation:UIInterfaceOrientationLandscapeRight duration:0.25f];
        )
        gEngine->engineToNativeThread( new AlertCallback() );
        return NULL; 
    }
    
	pool = [[NSAutoreleasePool alloc] init];
	usleep( 0 );

	uint poolRefreshCounter = 0;
	do
	{
		if( gEngine->paused )
		{
			// Sleep at 20 fps
			usleep( 50000 );
		}
		else
		{
			gEngine->updateEngineThread();
		}
		
		refreshReleasePool( &pool, &poolRefreshCounter, 1000 );
	} while( gEngine->running );
	[pool release];
	
	gEngine->engineThreadRunning = false;
	return NULL;
}


void* PosixJobsThread(void* data)
{	
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	uint poolRefreshCounter = 0;
	do
	{
		if( gEngine->paused )
		{
			// Sleep at 20 fps
			usleep( 50000 );
		}
		else
		{
			if( gEngine->updateJobsThread() )
            {
                usleep( 10 );
            }
            else
            {
                usleep( 200 );
            }
		}
		
		refreshReleasePool( &pool, &poolRefreshCounter, 1000 );
	} while( gEngine->running );
	[pool release];
	return NULL;
}


-(void)emptyThread
{
}


void createThread(void *(*start_routine)(void*), void *__restrict arg, int prioritySet, int priorityAdj)
{
	// Create the game thread using POSIX routines.
	pthread_attr_t attr;
	pthread_t posixThreadID;
	int returnVal;
	
	returnVal = pthread_attr_init( &attr );
	ASSERT( !returnVal );
	
	returnVal = pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
	ASSERT( !returnVal );
	
	returnVal = pthread_create( &posixThreadID, &attr, start_routine, arg );
	ASSERT( !returnVal );
	
	struct sched_param param;
	int policy;
	pthread_getschedparam( posixThreadID, &policy, &param );
	ASSERT( !returnVal );
	
	if( prioritySet != 0 )
	{
		param.sched_priority = prioritySet;
	}
	else if( priorityAdj != 0 )
	{
		param.sched_priority += priorityAdj;
	}
	ASSERT( param.sched_priority > 0 && param.sched_priority < 100 );
	
	returnVal = pthread_setschedparam( posixThreadID, policy, &param );
	ASSERT( !returnVal );
	
	returnVal = pthread_attr_destroy( &attr );
	ASSERT( !returnVal );
}


-(void)setup
{
	gEngine->setupNativeThread();
	
	// iPhone SDK recommends launching an empty NSThread when using POSIX threads with Cocoa applications
	[NSThread detachNewThreadSelector:@selector( emptyThread ) toTarget:self withObject:NULL]; 
	
	// Create the engine thread using POSIX routines.
	createThread( &PosixEngineThread, self, 0, +4 );
	createThread( &PosixJobsThread, self, 1, 0 );
	
	// Start the updating of native OS thread
	[updateTimer invalidate];
	updateTimer = [NSTimer scheduledTimerWithTimeInterval:NATIVETHREAD_INTERVAL target:self selector:@selector( updateNativeThread ) userInfo:NULL repeats:true];
}


-(void)updateNativeThread
{
	if( gEngine->engineThreadRunning )
	{	
        static bool firstRun = true;
        if( firstRun )
        {
            firstRun = false;
            
            CCAppManager::appManager->launchWindow();
            
            // Enable Multi Touch of the view
            self.multipleTouchEnabled = true;
            self.userInteractionEnabled = true;
        }
        
        [viewController detectOrientationUpdate:NATIVETHREAD_INTERVAL];
        
        if( gEngine->updateNativeThread() == false )
        {
        }
	}
}


// These are four methods touchesBegan, touchesMoved, touchesEnded, touchesCancelled and use to notify about touches and gestures
-(void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event 
{	
    CCDeviceControls *controls = (CCDeviceControls*)gEngine->controls;
	controls->touchBegin( touches, self );
}


-(void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event 
{
    CCDeviceControls *controls = (CCDeviceControls*)gEngine->controls;
	controls->touchMove( touches, self);
}


-(void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event 
{
    CCDeviceControls *controls = (CCDeviceControls*)gEngine->controls;
	controls->touchEnd( touches, event, self );
}


-(void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event 
{
    CCDeviceControls *controls = (CCDeviceControls*)gEngine->controls;
	controls->touchEnd( touches, event, self );
}


-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex 
{	
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"http://igrapher.com"]]; 
}

@end
