#ifndef TFT_ANIMATION_H_INCLUDED
#define TFT_ANIMATION_H_INCLUDED

#include <vector>
#include <SdFat.h>
#include "TFT_CoreApp.h"
#include "TFT_Timer.h"

#define COLOR_KEY   0xFFFF

using namespace std;
typedef struct 
{
    TFT_Rect rect;
    uint32_t time;
    uint16_t *data;
}TFT_FrameData;

typedef struct 
{
    uint32_t nbFrame;
    vector<TFT_FrameData> frames;
}TFT_AnimData;

class TFT_Anim : public TFT_Widget
{
    friend class TFT_CoreApp;
    public:
        TFT_Anim(): _haveColorKey(false),_colorKey(COLOR_KEY), _activate(false),_repeat(true)
                    ,_startFrame(true), _curFrame(0),_timer(0) {}
        void play() {_activate = true;}
        void stop() {_activate = false;}
        void repeat(bool rpt) { _repeat = rpt;}
        bool load(const char *path);
        void setColorKey(uint16_t color);
        void draw() override;
        ~TFT_Anim();

    private:
        bool _haveColorKey;
        uint16_t _colorKey;
        bool _activate, _repeat, _startFrame;
        uint32_t _curFrame;
        StopWatch _timer;
        TFT_AnimData _animData;
};


#endif