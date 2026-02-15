#ifndef TFT_CONTAINER_H_INCLUDED
#define TFT_CONTAINER_H_INCLUDED

#include <vector>
#include <algorithm>
#include <numeric>
#include "TFT_CoreApp.h"

typedef struct {
    TFT_Widget *widget;
    uint16_t marginLeft, marginTop, marginRight, marginBottom;
} TFT_GridElement;

class TFT_Container : public TFT_Widget
{
    public:
        TFT_Container(): _marginX(0),_marginY(0) {  }
        ~TFT_Container();
    
        void resize(uint16_t newWidth, uint16_t newHeight);
        void setMargin(uint16_t marginX, uint16_t marginY);
        bool addWidget(TFT_Widget* widget,uint8_t row,uint8_t col,uint16_t marginLeft
                                         , uint16_t marginRight,uint16_t marginTop,uint16_t marginBottom);
        bool addWidget(TFT_Widget* widget, uint8_t row, uint8_t col);
        void setWidgetMargin(uint8_t row, uint8_t col, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
        uint8_t getNbRow();
        vector<uint16_t> getMaxHeightByRow();
        void updateLayout();
        void setPos(uint16_t x, uint16_t y) override;
        virtual void draw() override;

    protected:
        void ensureGridSize(uint8_t row, uint8_t col);
        TFT_GridElement createVoidElement();

        uint16_t _marginX, _marginY;
        vector<vector<TFT_GridElement>> _grid;
};

#endif
