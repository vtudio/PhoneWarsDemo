/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCAppDelegate.mm
 *-----------------------------------------------------------
 */

#import "CCDefines.h"
#import "CCAppDelegate.h"
#import "CCAppManager.h"


@implementation CCAppDelegate


-(id)init
{
	self = [super init];
	if( self != NULL )
	{
        new CCAppManager();
	}
	
	return self;
}


-(void)applicationDidFinishLaunching:(UIApplication*)application
{
}


-(BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
    // Override point for customization after application launch.
    CCAppManager::appManager->startup();
    return true;
}


-(void)dealloc 
{	
    delete CCAppManager::appManager;
	[super dealloc];
}


-(void)applicationWillTerminate:(UIApplication*)application
{
    CCAppManager::appManager->shutdown();
}


-(void)applicationWillResignActive:(UIApplication*)application
{
    CCAppManager::appManager->pause();
}


-(void)applicationDidBecomeActive:(UIApplication*)application
{
    CCAppManager::appManager->resume();
}


-(void)applicationDidReceiveMemoryWarning:(UIApplication*)application
{
	NSLog( @"applicationDidReceiveMemoryWarning" );
}  

@end

