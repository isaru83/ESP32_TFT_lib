#include "TFT_Animation.h"

extern TFT_CoreApp *TFT_CoreAppPtr;

TFT_Anim::~TFT_Anim()
{
    for (auto &frame : _animData.frames)
    {
        if (frame.data)
            free(frame.data);
    }
    _animData.frames.clear();
}


bool TFT_Anim::load(const char *path)
{
    SdFat *sd = TFT_CoreAppPtr->getSdPtr();
    auto f = sd->open(path, O_RDONLY);   // O_RDONLY pour lecture binaire
    if (!f)
        return false;
    if (f.read((uint8_t*)&_animData.nbFrame, sizeof(uint32_t)) != sizeof(uint32_t))
    {
        f.close();
        return false;
    }

    for (uint32_t i = 0; i < _animData.nbFrame; i++)
    {
        TFT_FrameData frameData;
        if (f.read((uint8_t*)&frameData.rect.x, sizeof(uint32_t)) != sizeof(uint32_t) ||
            f.read((uint8_t*)&frameData.rect.y, sizeof(uint32_t)) != sizeof(uint32_t) ||
            f.read((uint8_t*)&frameData.rect.w, sizeof(uint32_t)) != sizeof(uint32_t) ||
            f.read((uint8_t*)&frameData.rect.h, sizeof(uint32_t)) != sizeof(uint32_t) ||
            f.read((uint8_t*)&frameData.time, sizeof(uint32_t)) != sizeof(uint32_t))
        {
            f.close();
            return false;
        }

        uint32_t dataSize = frameData.rect.w * frameData.rect.h * sizeof(uint16_t);
        frameData.data = (uint16_t*)ps_malloc(dataSize);
        if (!frameData.data)
        {
            f.close();
            return false;
        }

        size_t n = f.read((uint8_t*)frameData.data, dataSize * sizeof(uint16_t));
        if (n != dataSize * sizeof(uint16_t))
        {
            free(frameData.data);
            f.close();
            return false;
        }

        _animData.frames.push_back(frameData);
    }

    f.close();
    return true;
}

void TFT_Anim::draw()
{
    if(_activate)
    {
        if(_startFrame)
        {
            uint32_t time = _animData.frames[_curFrame].time;
            _timer.setDelay(time);
            _timer.start();
            _startFrame = false;
        }
        TFT_Rect rect;
        rect.x = _rect.x + _animData.frames[_curFrame].rect.x;
        rect.y = _rect.y + _animData.frames[_curFrame].rect.y;
        rect.w = _animData.frames[_curFrame].rect.w;
        rect.h = _animData.frames[_curFrame].rect.h;

        if(_haveColorKey)
            TFT_CoreAppPtr->pushImg(_animData.frames[_curFrame].data,rect,_colorKey);
        else
            TFT_CoreAppPtr->pushImg(_animData.frames[_curFrame].data,rect);
            
        if(_timer.getTrigg())
        {
            _timer.reset();
            _curFrame++;
            _startFrame = true;
            if(_curFrame >= _animData.nbFrame)
            {
                if(_repeat)
                    _curFrame = 0;
                else
                {
                    _activate = false;
                    _curFrame = 0;
                }
            }
        }
    }
}

void TFT_Anim::setColorKey(uint16_t color)
{
    _haveColorKey = true;
    _colorKey = color;
}