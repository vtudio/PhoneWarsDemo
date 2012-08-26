/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCPlatform.mm
 *-----------------------------------------------------------
 */

#import "CCDefines.h"
#import "CCGLView.h"

#import <pthread.h>

void CCNativeThreadLock()
{
//    pthread_t threadID = pthread_self();
//    DEBUGLOG( "Locked thread %i \n", threadID );
    pthread_mutex_lock( &gView->nativeThreadMutex->mutex );
}


void CCNativeThreadUnlock()
{
//    pthread_t threadID = pthread_self();
//    DEBUGLOG( "Unlocked thread %i \n", threadID );
    pthread_mutex_unlock( &gView->nativeThreadMutex->mutex );
}


void CCJobsThreadLock()
{
    //    pthread_t threadID = pthread_self();
    //    DEBUGLOG( "Locked thread %i \n", threadID );
    pthread_mutex_lock( &gView->jobsThreadMutex->mutex );
}


void CCJobsThreadUnlock()
{
    //    pthread_t threadID = pthread_self();
    //    DEBUGLOG( "Unlocked thread %i \n", threadID );
    pthread_mutex_unlock( &gView->jobsThreadMutex->mutex );
}