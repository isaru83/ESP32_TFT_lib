#ifndef TFT_TXT_H_INCLUDED
#define TFT_TXT_H_INCLUDED

#include "TFT_CoreApp.h"
#include <TFT_eSPI.h>

#define NB_CHAR_MAX  32

class TFT_Text : public TFT_Widget
{
public:
    TFT_Text(const char *txt, uint16_t color, TFT_Rect rect);//Impose une taille
    ~TFT_Text();
    void draw();
    void resize(uint16_t newWidth, uint16_t newHeight){}
    void resizeText(uint8_t newSize);
    void setText(const char *txt);
    void setText(float val);
    void setText(int val);

    void RxSlotSetText(void *msg); //Reception signal
    
private:
    uint16_t _color;
    TFT_eSprite _sprite;
    uint8_t _size;
    char _txt[NB_CHAR_MAX];
};

#endif
