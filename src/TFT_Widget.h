#ifndef TFT_WIDGET_H_INCLUDED
#define TFT_WIDGET_H_INCLUDED
#include <Arduino.h>

typedef struct 
{
  uint16_t x, y, w, h;
}TFT_Rect;

typedef struct 
{
  uint16_t w,h;
}TFT_Size;

class TFT_Widget
{
    public:
      TFT_Widget(): _rect({0,0,0,0}) {}
      virtual ~TFT_Widget() {};
      virtual void setPos(uint16_t x, uint16_t y) {_rect.x = x; _rect.y = y;}
      virtual void setSize(uint16_t w, uint16_t h) {_rect.w  = w; _rect.h = h;}
      virtual TFT_Rect getRect() { return _rect; }
      TFT_Rect *getRectPtr() { return &_rect;}
      virtual void setRect(TFT_Rect rect) { _rect = rect; }
      virtual void setX(uint16_t x) {_rect.x = x; }
      virtual void setY(uint16_t y) {_rect.y = y; }
      virtual uint16_t getX(uint16_t x) {return _rect.x; }
      virtual uint16_t getY(uint16_t y) {return _rect.y; }
      virtual void draw()= 0;

    protected:
      TFT_Rect _rect;

};

#endif