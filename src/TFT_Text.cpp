#include "TFT_Text.h"

extern TFT_CoreApp *TFT_CoreAppPtr;

TFT_Text::TFT_Text(const char *txt, uint16_t color, TFT_Rect rect)
    : _color(color), _sprite(TFT_CoreAppPtr->getTftPtr())
{
    strncpy(_txt, txt, NB_CHAR_MAX - 1);
    _txt[NB_CHAR_MAX - 1] = '\0';

    TFT_eSPI *tft = TFT_CoreAppPtr->getTftPtr();
    tft->setFreeFont(&FreeMonoBold18pt7b);

    // Calcul dimensions
    _rect.w = rect.w;
    _rect.h = rect.h;

    _sprite.createSprite(_rect.w, _rect.h);
    _sprite.setTextColor(_color);
    _sprite.setTextDatum(TR_DATUM);
    _sprite.setFreeFont(&FreeMonoBold18pt7b);

    _rect.x = rect.x;
    _rect.y = rect.y;
}

TFT_Text::~TFT_Text()
{
    if (_sprite.created())
        _sprite.deleteSprite();
}

void TFT_Text::draw()
{
    int x = _rect.w - 1;
    int y = 0;
    _sprite.fillSprite(TFT_BLACK);
    _sprite.drawString(_txt, _rect.w - 1,0);

    uint16_t *spriteData = (uint16_t *)_sprite.getPointer();
    if (!spriteData)
    {
        Serial.println("sprite->getPointer() NULL");
        return;
    }

    uint16_t *buf = TFT_CoreAppPtr->getTextBuffer();
    uint32_t len = _rect.w * _rect.h;

    for (uint32_t i = 0; i < len; i++) 
        buf[i] = TFT_CoreApp::swapBytes(spriteData[i]);

    TFT_CoreAppPtr->pushImg(buf, _rect);
}

void TFT_Text::setText(const char *txt)
{
    if(strlen(txt) < NB_CHAR_MAX)
        strcpy(_txt,txt);
}

void TFT_Text::setText(float val)
{
    char buf[16];
    sprintf(buf,"%.3f",val);
    setText(buf);
}
void TFT_Text::setText(int val)
{
    char buf[16];
    sprintf(buf,"%d",val);
    setText(buf);
}

void TFT_Text::resizeText(uint8_t newSize)
{
    if (newSize >= 4) 
        newSize = 3; // Sécurité

    _size = newSize;

    if (_sprite.created())
        _sprite.deleteSprite();

    // Tableau de polices disponibles
    const GFXfont* fonts[] = {
        &FreeSans9pt7b,
        &FreeSans12pt7b,
        &FreeSans18pt7b,
        &FreeSans24pt7b
    };

    const GFXfont* selectedFont = fonts[_size];

    TFT_eSPI* tft = TFT_CoreAppPtr->getTftPtr();
    tft->setFreeFont(selectedFont);

    _rect.w = tft->textWidth(_txt);
    _rect.h = tft->fontHeight();

    _sprite.createSprite(_rect.w, _rect.h);
    _sprite.setTextColor(_color, TFT_WHITE);
    _sprite.setFreeFont(selectedFont);
    _sprite.setTextDatum(TL_DATUM);

    _sprite.fillSprite(TFT_WHITE);
    _sprite.drawString(_txt, 0, 0);
}

void TFT_Text::RxSlotSetText(void *msg)
{
    if(!msg)
        return;
    const char *newText = static_cast<const char *>(msg);
    setText(newText);
}