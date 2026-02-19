#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <Arduino.h>

class FrameRate
{
    public:
        FrameRate(unsigned int time) : _time(time), _prevTime(0) {}
        void run()
        {
            unsigned long now = millis();
            if (now - _prevTime >= _time)
            {
                _prevTime = now;
                return;
            }
        
            while (millis() - _prevTime < _time)
            ;
            _prevTime = millis();
        }

    private:
        unsigned int _time;         
        unsigned long _prevTime;    
};

// Classe pour gérer un timer non bloquant
class StopWatch
{
    public:
        StopWatch(unsigned long delay) 
            : _delay(delay), _startTime(0), _remainTime(0), _isStartedFlag(false) {}

        void start()
        {
            if (!_isStartedFlag)
            {
                _startTime = millis();
                _remainTime = 0;
                _isStartedFlag = true;
            }
        }

        bool getTrigg()
        {
            if (!_isStartedFlag)
                return false;

            unsigned long now = millis();
            _remainTime = now - _startTime;

            if (_remainTime >= _delay)
                return true;
    
            return false;
        }

        void reset()
        {
            _isStartedFlag = false;
            _remainTime = 0;
        }

        void reinit()
        {
            reset();
            start();
        }

        void setDelay(unsigned long delay)
        {
            _delay = delay;
        }

        unsigned int remainTimeSecond() const
        {
            if (_isStartedFlag && _delay > _remainTime)
                return (unsigned int)((_delay - _remainTime) / 1000);
            return 0;
        }

        bool isStarted() const
        {
            return _isStartedFlag;
        }

    private:
        unsigned long _delay;      
        unsigned long _startTime;  
        unsigned long _remainTime;  
        bool _isStartedFlag;     
};

#endif // TIMER_H_INCLUDED
