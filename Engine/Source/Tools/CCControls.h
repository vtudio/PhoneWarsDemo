/*-----------------------------------------------------------
 * 2c - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2010 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license.
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *-----------------------------------------------------------
 * File Name   : CCControls.h
 * Description : Cross platform controls interface.
 *
 * Created     : 01/03/10
 * Author(s)   : Ashraf Samy Hegab
 *-----------------------------------------------------------
 */

#ifndef __CCCONTROLS_H__
#define __CCCONTROLS_H__


#define CC_DOUBLE_TAP_THRESHOLD 0.2f
#define CC_MIN_MOVEMENT_THRESHOLD 0.1f


enum CCTouchAction
{
	touch_pressed,
    touch_movingHorizontal,
    touch_movingVertical,
	touch_moving,
	touch_released,
	touch_lost
};


enum CCTwoTouchAction
{
    twotouch_unassigned,
    twotouch_zooming,
    twotouch_rotating,
};


struct CCScreenTouches
{
	CCScreenTouches()
	{
		lastTouch = usingTouch = NULL;	
		timeHeld = lastTimeReleased = 0.0f;
	}
	
    const CCPoint averageLastDeltas() const;
	
	void *usingTouch, *lastTouch;
    CCPoint startPosition, position, delta, totalDelta, lastTotalDelta;
	float timeHeld, lastTimeReleased;
	
    enum { max_last_deltas = 50 };
    struct TimedDelta
    {
        TimedDelta()
        {
            time = 0.0f;
        }

        void clear()
        {
            time = 0.0f;
            delta = CCPoint();
        }

        float time;
        CCPoint delta;
    };
    TimedDelta lastDeltas[max_last_deltas];
};



class CCControls
{
public:
	CCControls();
	
	void render();
	
	// Synchronizes the controls
	void update(const CCTime &time);
    
    // Update our touch logic
    static void UpdateTouch(CCScreenTouches &touch, const CCTime &time);
    
protected:
   	void unTouch(void *touch);
    
public:
    static bool DetectZoomGesture(const CCScreenTouches &touch1, const CCScreenTouches &touch2);
    static bool DetectRotateGesture(const CCScreenTouches &touch1, const CCScreenTouches &touch2);
    static bool TouchActionMoving(const CCTouchAction touchAction);
    
    const CCScreenTouches* getScreenTouches() { return screenTouches; }
    static const CCPoint& GetTouchMovementThreashold() { return touchMovementThreashold; }
    static void RefreshTouchMovementThreashold();
    static void SetDPI(const float x, const float y);
	
public:
	enum { numberOfTouches = 2 };
    
protected:
	bool inUse;
	CCScreenTouches screenTouches[numberOfTouches];
    static CCPoint touchMovementPixels;
    static CCPoint touchMovementThreashold;
};


#endif // __CONTROLS_H_
