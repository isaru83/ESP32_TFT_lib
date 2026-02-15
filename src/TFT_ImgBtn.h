#ifndef TFT_IMGBTN_H_INCLUDED
#define TFT_IMGBTN_H_INCLUDED

#include "TFT_AbstractBtn.h"
#include "TFT_Image.h"

class TFT_ImgBtn : public TFT_AbstractBtn
{
    public:
        TFT_ImgBtn(const char *pathNormal,const char *pathPressed,uint16_t x,uint16_t y): TFT_AbstractBtn(x, y),
                                            _imgNormal(pathNormal),_imgPressed(pathPressed),_pressed(false)
        {
            _imgNormal.load(pathNormal);
            _imgPressed.load(pathPressed);
            _rect.w = _imgNormal.getRectPtr()->w;
            _rect.h = _imgNormal.getRectPtr()->h;
            setPos(x,y);
        }
        void resize(uint16_t newWidth, uint16_t newHeight) 
        {
            _imgNormal.resize(newWidth,newHeight);
            _imgPressed.resize(newWidth,newHeight);
            setRect(_imgNormal.getRect());
        }
        void setPos(uint16_t x, uint16_t y) override
        {
            _rect.x = x;
            _rect.y = y;
            _imgPressed.setPos(x,y);
            _imgNormal.setPos(x,y);
        }

        void draw() override
        {
            // On met à jour l'état AVANT d'afficher
            update();
            if (_pressed)
            {
                _imgPressed.draw();
                WEmit(this,(WsgnlFctnPtr)&TFT_ImgBtn::TXisPressed,&_pressed);
            }
            else
                _imgNormal.draw();
        }

        void TXisPressed(void *msg){}

    protected:
    void update()
    {
        bool currentlyTouched = haveEvent();  // vrai si doigt dans le bouton CE frame

        if (currentlyTouched && !_wasTouchedLastFrame)
        {
            // → Événement DOWN détecté (front montant)
            _pressed = true;
            if (_callback)
                _callback();                  // ← CLIC UNIQUE ICI
        }
        else if (!currentlyTouched && _wasTouchedLastFrame)
        {
            // → Événement UP détecté (front descendant)
            _pressed = false;
        }
        // Sinon : doigt reste posé ou reste levé → rien

        _wasTouchedLastFrame = currentlyTouched;
    }

        TFT_Image _imgNormal;
        TFT_Image _imgPressed;
        bool _pressed, _wasTouchedLastFrame = false;
};

#endif
