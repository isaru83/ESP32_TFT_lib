#ifndef TFT_IMAGE_H_INCLUDED
#define TFT_IMAGE_H_INCLUDED

#include <Arduino.h>
#include "TFT_CoreApp.h"
#include <SdFat.h>

class TFT_Image : public TFT_Widget
{
  friend class TFT_CoreApp;
  public:
    TFT_Image() : _data(nullptr), _colorKey(0),_haveColorKey(false) {}
    TFT_Image(const char *path);
    TFT_Image(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t *data);
    TFT_Image(TFT_Rect rect) {  setRect(rect);}
    ~TFT_Image();

    void setColorKey(uint16_t colorKey);
    bool load(const char *path);
    void draw();    

    uint16_t *getData() { return _data; }
    uint16_t getWidth() { return _rect.w; }
    uint16_t getHeight() { return _rect.h; }
    void resize(uint16_t newWidth, uint16_t newHeight);

  private:
    uint16_t *_data;
    uint16_t _colorKey;
    bool _haveColorKey;
};

#endif