#ifndef TFT_ABSTRACTBTN_H_INCLUDED
#define TFT_ABSTRACTBTN_H_INCLUDED

#include "TFT_CoreApp.h"
#include <functional>

extern TFT_CoreApp *TFT_CoreAppPtr;

class TFT_AbstractBtn : public TFT_Widget
{
public:
    TFT_AbstractBtn(uint16_t x, uint16_t y)
    {
        _rect.x = x;
        _rect.y = y;
    }
    // Enregistre la fonction callback
    void setCallback(std::function<void()> cb)
    {
        _callback = cb;
    }
    // A appeler dans la boucle principale
    void update()
    {
        if (haveEvent() && _callback)
        {
            _callback();
        }
    }

    virtual void draw() = 0;

protected:
    // Vérifie si l'événement tactile concerne ce bouton
    bool haveEvent()
    {
        if (TFT_CoreAppPtr->event())
        {
            TouchEvent *event = TFT_CoreAppPtr->getTouchEvent();
            if (TFT_CoreApp::Collision(event->x, event->y, _rect))
                return true;
        }
        return false;
    }

    std::function<void()> _callback;
};

#endif
