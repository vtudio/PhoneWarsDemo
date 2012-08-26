/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 - 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCAppEngine.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCAppManager.h"
#include "Game.h"


void CCAppEngine::start()
{
#ifdef IOS
    CCAppManager::SetOrientation( 270.0f, false );
#endif
    
    gEngine->textureManager->loadFont( "PressStartK", "Resources/common/fonts/" );
    
    addScene( new SceneAndroidsManager() );
    
//    CCAddFlag( gEngine->renderFlags, render_collisionBoxes );
//    CCAddFlag( gEngine->renderFlags, render_collisionTrees );
//    CCAddFlag( gEngine->renderFlags, render_pathFinder );
}


bool CCAppEngine::isOrientationSupported(const float angle)
{
    if( angle == 270.0f || angle == 90.0f )
    {
        return true;
    }
    return false;
}
