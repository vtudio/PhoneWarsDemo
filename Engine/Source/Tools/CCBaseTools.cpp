/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCBaseTypes.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include <assert.h>

#ifdef ANDROID
#include "CCJNI.h"
#endif


#ifdef DEBUGON

void CCDebugAssert(const bool condition, const char *file, const int line, const char *message)
{
    if( !condition )
    {
        DEBUGLOG( "%s %i \n", file, line );
        fflush( stdout );
        if( message )
        {
            DEBUGLOG( "ASSERT: %s \n", message );
        }

        // Root call to Java on Android for ease of debugging
#ifdef ANDROID
        CCJNI::Assert( file, line, message );
#endif
        assert( condition );
    }
}

#endif
