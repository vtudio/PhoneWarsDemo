/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCInterpolators.h
 * Description : Interpolators for various curves.
 *
 * Created     : 30/04/11
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCINTERPOLATORS_H__
#define __CCINTERPOLATORS_H__


class CCInterpolator : public CCUpdater
{
public:
    CCInterpolator()
    {
        speed = 1.0f;
    }

    virtual ~CCInterpolator()
    {
#ifdef DEBUGON
        destructCalled = true;
#endif

        onInterpolated.deleteObjectsAndList();
    }
    
    void setDuration(const float duration);

public:
    LAMBDA_SIGNAL onInterpolated;
    
protected:
    float speed;
};



class CCInterpolatorSin2Curve : public CCInterpolator
{
public:
    CCInterpolatorSin2Curve()
    {  
        current = NULL;
        amount = 1.0f;
    }
    
    CCInterpolatorSin2Curve(float *inCurrent, const float inTarget)
    {
        setup( inCurrent, inTarget, true );
    }
    
    bool equals(float *inCurrent, const float inTarget);
    void setup(float *inCurrent, const float inTarget, const bool force=true);

    void setCurrent(float *inCurrent);
    void setTarget(float inTarget);
    
    // Call when ready to start the interpolation
    void ready();
    
    bool incrementAmount(const float delta);
    void setAmount(const float amount)
    {
        this->amount = amount;
    }
    virtual float calculatePercentage();
    void updateInterpolation(const float percentage);
    virtual bool update(const float delta);
    
    bool isUpdating()
    {
        return amount < 1.0f;
    }
    inline const float* getCurrent() const { return current; }
    inline float getAmount() const { return amount; }
    inline float getTarget() const { return target; }
    
protected:
    float *current;
    float target;
    float start;
    float length;
    float amount;
};



class CCInterpolatorX2Curve : public CCInterpolatorSin2Curve
{
public:
    float calculatePercentage();
};


class CCInterpolatorX3Curve : public CCInterpolatorSin2Curve
{
public:
    float calculatePercentage();
};


class CCInterpolatorSinCurve : public CCInterpolatorSin2Curve
{
public:
    float calculatePercentage();
};


class CCInterpolatorLinear : public CCInterpolatorSin2Curve
{
public:
    float calculatePercentage();
};



template <typename T> 
class CCInterpolatorXY : public CCInterpolator
{
public:
    void setup(float *inX, float *inY, const float target)
    {
        x.setup( inX, target );
        y.setup( inY, target );
    }
    
    void setup(float *inX, float *inY, const float targetX, const float targetY)
    {
        
        if( x.getCurrent() != inX || x.getTarget() != targetX )
        {
            x.setup( inX, targetX );
        }
        
        
        if( y.getCurrent() != inY || y.getTarget() != targetY )
        {
            y.setup( inY, targetY );
        }
    }
    
    bool update(const float delta)
    {
        float deltaSpeed = delta * speed;
        bool updating = x.update( deltaSpeed );
        updating |= y.update( deltaSpeed );
        return updating;
    }
    
protected:
    T x,y;
};



template <typename T> 
class CCInterpolatorV3 : public CCInterpolator
{
public:
    CCInterpolatorV3() {}
    
    CCInterpolatorV3(CCVector3 *inCurrent, const CCVector3 target, CCLambdaCallback *inCallback=NULL)
    {
        setup( inCurrent, target, inCallback );
    }
    
    bool equals(CCVector3 *inCurrent, const CCVector3 target)
    {
        if( x.equals( &inCurrent->x, target.x ) &&
            y.equals( &inCurrent->y, target.y ) &&
            z.equals( &inCurrent->z, target.z ) )
        {
            return true;
        }
        return false;
    }
    
    void clear()
    {
        x.setCurrent( NULL );
        y.setCurrent( NULL );
        z.setCurrent( NULL );
    }

    
    void setup(CCVector3 *inCurrent, const CCVector3 target, CCLambdaCallback *inCallback=NULL)
    {
        ASSERT( inCurrent != NULL );
        
        x.setup( &inCurrent->x, target.x );
        y.setup( &inCurrent->y, target.y );
        z.setup( &inCurrent->z, target.z );
        
        onInterpolated.deleteObjects();
        if( inCallback != NULL )
        {
            onInterpolated.add( inCallback );
        }
    }
    
    void ready()
    {
        x.ready();
        y.ready();
        z.ready();
    }
    
    bool update(const float delta)
    {
        float deltaSpeed = delta * speed;
        bool updating = x.update( deltaSpeed );
        updating |= y.update( deltaSpeed );
        updating |= z.update( deltaSpeed );
        
        if( onInterpolated.length > 0 )
        {
            if( updating == false )
            {
                LAMBDA_EMIT_ONCE( onInterpolated );
            }
        }
        
        return updating;
    }
    
    const CCVector3 getAmount() const { return CCVector3( x.getAmount(), y.getAmount(), z.getAmount() ); }
    const CCVector3 getTarget() const { return CCVector3( x.getTarget(), y.getTarget(), z.getTarget() ); }
    
    bool isUpdating()
    {
        if( x.getAmount() < 1.0f || y.getAmount() < 1.0f || z.getAmount() < 1.0f )
        {
            return true;
        }
        return false;
    }
    
protected:
    T x,y,z;
};



template <typename T> 
class CCInterpolatorListV3 : public CCInterpolator
{
public:
    virtual ~CCInterpolatorListV3()
    {
        clear();
    }
    
    void clear()
    {
        interpolators.deleteObjectsAndList();
    }
    
    
    // replace: Deletes all the other pending interpolations and pushes this target to the front
    bool pushV3(CCVector3 *inCurrent, const CCVector3 target, const bool replace=false, CCLambdaCallback *inCallback=NULL)
    {
        if( interpolators.length > 0 )
        {
            if( replace )
            {
                bool found = false;
                for( int i=0; i<interpolators.length; ++i )
                {
                    CCInterpolatorV3<T> *interpolator = interpolators.list[i];
                    if( interpolator->equals( inCurrent, target ) )
                    {
                        found = true;
                        if( i != 0 )
                        {
                            interpolator->ready();
                        }
                    }
                    else
                    {
                        interpolators.remove( interpolator );
                        delete interpolator;
                    }
                }
                
                if( found )
                {
                    return false;
                }
            }
            else
            {
                for( int i=0; i<interpolators.length; ++i )
                {
                    CCInterpolatorV3<T> *interpolator = interpolators.list[i];
                    if( interpolator->equals( inCurrent, target ) )
                    {
                        return false;
                    }
                }
            }
        }
        
        if( *inCurrent != target )
        {
            interpolators.add( new CCInterpolatorV3<T>( inCurrent, target, inCallback ) );
        }
        return true;
    }
    
    bool update(const float delta);
    
    bool finished() { return interpolators.length == 0; }
    
    const CCVector3 getTarget() const
    {
        if( interpolators.length > 0 )
        {
            return interpolators.last()->getTarget();
        }
        return CCVector3();
    }
    
public:
    CCList< CCInterpolatorV3< T > > interpolators;
};


class CCInterpolatorLinearColour : public CCInterpolator
{
public:
    CCInterpolatorLinearColour()
    {
        current = NULL;
        updating = false;
    }
    
    CCInterpolatorLinearColour(CCColour *inCurrent, const CCColour inTarget)
    {
        setup( inCurrent, inTarget );
    }
    
    bool equals(CCColour *inCurrent, const CCColour inTarget)
    { 
        // Ignore if we're already doing this
        if( current != inCurrent || target != inTarget )
        {
            return false;
        }
        return true;
    }
    
    void setup(CCColour *inCurrent, const CCColour inTarget)
    {
        current = inCurrent;
        target = inTarget;
        updating = true;
        
        if( onInterpolated.length > 0 )
        {
            onInterpolated.deleteObjects();
        }
    }
    
    bool update(const float delta);
    
    bool isUpdating()
    {
        return updating;
    }
    
    void setTarget(const CCColour inTarget, CCLambdaCallback *inCallback=NULL)
    {
        setup( current, inTarget );
        if( inCallback != NULL )
        {
            onInterpolated.add( inCallback );
        }
    }

    void setTarget(const CCColour *inTarget, CCLambdaCallback *inCallback=NULL)
    {
        setTarget( *inTarget, inCallback );
    }
    
    void setTargetColour(const float grey)
    {
        target.red = grey;
        target.green = grey;
        target.blue = grey;
        updating = true;
    }

    void setTargetAlpha(const float inTargetAlpha, CCLambdaCallback *inCallback=NULL)
    {
        if( target.alpha != inTargetAlpha )
        {
            target.alpha = inTargetAlpha;
            setTarget( target, inCallback );
        }
        else
        {
            if( inCallback != NULL )
            {
                onInterpolated.add( inCallback );
            }
        }
    }

    inline const CCColour& getTarget() const { return target; }
    
protected:
    bool updating;
    CCColour *current;
    CCColour target;
};



class CCTimer : public CCUpdater
{
public:
    CCTimer()
    {
        time = 0.0f;
        updating = false;
        interval = 0.0f;
    }
    
    ~CCTimer()
    {
        onTime.deleteObjects();
    }
    
    bool update(const float delta);
    virtual void finish();
    void start(const float timeout);
    void stop();
    void restart();
    
    bool updating;
    float time;
    float interval;
    LAMBDA_SIGNAL onTime;
};


#endif // __CCInterpolators_H__
