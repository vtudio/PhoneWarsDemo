/*-----------------------------------------------------------
 * Copyright 2011 - 2012 SoftPoetry
 * http://softpoetry.com/
 *-----------------------------------------------------------
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : SceneGameManager.h
 * Description : Manager for our level events
 *
 * Created     : 29/04/12
 *-----------------------------------------------------------
 */


struct EmitterData
{
    EmitterData(CCLambdaCallback *spawnCallback);
    ~EmitterData();
    void update(const float delta);
    const bool isUpdating();
    
    CCText spawnType;
    float spawnTimer;
    float spawnFrequency;
    CCLambdaCallback *spawnCallback;
    
    float timeRemaining;
    int spawnsRemaining;
    
};


struct WaveData
{
    virtual ~WaveData();
    
    void update(const float delta);
    const bool isUpdating();
    
    CCList<EmitterData> emitters;
};

struct SceneGameManager
{
    SceneGameManager();
    virtual ~SceneGameManager();
    
    void update(const float delta);
    const bool isUpdating();
    const bool nextWave();
    
    void addWave();
    void addEmitter(const char *type, float frequency, const int spawns, CCLambdaCallback *spawnCallback);
    void addEmitterTimed(const char *type, float frequency, const float time, CCLambdaCallback *spawnCallback);
    
    CCList<WaveData> waves;
    int currentWaveIndex;
};
