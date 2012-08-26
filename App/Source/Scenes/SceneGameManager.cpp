/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : SceneGameManager.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "Game.h"


// SceneGameManager
SceneGameManager::SceneGameManager()
{
    currentWaveIndex = 0;
    
    addWave();
}


SceneGameManager::~SceneGameManager()
{
    waves.deleteObjects();
}


void SceneGameManager::update(const float delta)
{
    if( currentWaveIndex < waves.length )
    {
        WaveData *currentWave = waves.list[currentWaveIndex];
        currentWave->update( delta );
    }
}


const bool SceneGameManager::isUpdating()
{
    if( currentWaveIndex < waves.length )
    {
        WaveData *currentWave = waves.list[currentWaveIndex];
        return currentWave->isUpdating();
    }
    return false;
}


const bool SceneGameManager::nextWave()
{
    currentWaveIndex++;
    if( currentWaveIndex < waves.length )
    {
        return true;
    }
    return false;
}


void SceneGameManager::addWave()
{
    waves.add( new WaveData() );
}


void SceneGameManager::addEmitter(const char *type, float frequency, const int spawns, 
                                  CCLambdaCallback *spawnCallback)
{
    ASSERT( waves.length > 0 );
    WaveData *wave = waves.last();
    
    EmitterData *emitter = new EmitterData( spawnCallback );
    emitter->spawnType = type;
    emitter->spawnFrequency = frequency;
    
    ASSERT( spawns > 0 );
    emitter->spawnsRemaining = spawns;
    
    wave->emitters.add( emitter );
}


void SceneGameManager::addEmitterTimed(const char *type, float frequency, const float time,
                                       CCLambdaCallback *spawnCallback)
{
    ASSERT( waves.length > 0 );
    WaveData *wave = waves.last();
    
    EmitterData *emitter = new EmitterData( spawnCallback );
    emitter->spawnType = type;
    emitter->spawnFrequency = frequency;
    
    ASSERT( time > 0.0f );
    emitter->timeRemaining = time;
    
    wave->emitters.add( emitter );
}


// WaveData
WaveData::~WaveData()
{
    emitters.deleteObjects();
}


void WaveData::update(const float delta)
{
    for( int i=0; i<emitters.length; ++i )
    {
        EmitterData *emitter = emitters.list[i];
        emitter->update( delta );
    }
}


const bool WaveData::isUpdating()
{
    for( int i=0; i<emitters.length; ++i )
    {
        EmitterData *emitter = emitters.list[i];
        if( emitter->isUpdating() )
        {
            return true;
        }
    }
    
    return false;
}



// EmitterData
EmitterData::EmitterData(CCLambdaCallback *spawnCallback)
{   
    spawnTimer = 0.0f;
    ASSERT( spawnCallback != NULL );
    this->spawnCallback = spawnCallback;
    
    timeRemaining = -1.0f;
    spawnsRemaining = -1;
}


EmitterData::~EmitterData()
{
    delete spawnCallback;
}


void EmitterData::update(const float delta)
{
    if( timeRemaining > 0.0f )
    {
        timeRemaining -= delta;
        if( timeRemaining < 0.0f )
        {
            timeRemaining = 0.0f;
        }
    }
    
    spawnTimer += delta;   
    if( spawnTimer > spawnFrequency )
    {
        spawnTimer -= spawnFrequency;
        
        if( timeRemaining > 0.0f )
        {
            // Spawn
            spawnCallback->run();
        }
        else if( spawnsRemaining > 0 )
        {
            spawnsRemaining--;
            
            // Spawn
            spawnCallback->run();
        }
    }
}


const bool EmitterData::isUpdating()
{
    if( timeRemaining > 0.0f )
    {
        return true;
    }
    
    if( spawnsRemaining > 0 )
    {
        return true;
    }
    
    return false;
}