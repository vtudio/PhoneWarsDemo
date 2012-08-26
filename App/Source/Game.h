#ifndef __GAME_H__
#define __GAME_H__


enum DeviceType
{
    Device_iOS,
    Device_Android
};


#include "ObjectCollideableFloor.h"
#include "ObjectCollideableWall.h"
#include "ObjectCollideableParticle.h"

#include "ObjectIndicator.h"
#include "PickupBase.h"
#include "WeaponBase.h"

#include "ObjectMoveableBullet.h"

#include "CharacterBase.h"
#include "CharacterPlayer.h"
#include "CharacterPlayerAndroid.h"
#include "CharacterPlayerBurger.h"

#include "CharacterUpdaterPlayer.h"
#include "CharacterUpdaterAI.h"

#include "SceneGameManager.h"
#include "SceneGameSyndicate.h"
#include "SceneGameBurgersDeathmatch.h"

#include "SceneSplashScreen.h"
#include "SceneBackground.h"
#include "ScenePlayManager.h"
#include "SceneAndroidsManager.h"

#include "SceneGameUI.h"


#endif // __GAME_H__
