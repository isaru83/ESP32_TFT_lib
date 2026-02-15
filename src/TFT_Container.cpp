#include "TFT_Container.h"

extern TFT_CoreApp *TFT_CoreAppPtr;

TFT_Container::~TFT_Container()
{
    
}

TFT_GridElement TFT_Container::createVoidElement()
{
    TFT_GridElement e;
    e.widget = nullptr;
    e.marginLeft = e.marginTop = e.marginRight = e.marginBottom = 0;
    return e;
}

void TFT_Container::ensureGridSize(uint8_t row, uint8_t col)
{
    while (_grid.size() <= row)
        _grid.push_back(vector<TFT_GridElement>());

    while (_grid[row].size() <= col)
        _grid[row].push_back(createVoidElement());
}

bool TFT_Container::addWidget(TFT_Widget* widget, uint8_t row, uint8_t col)
{
    ensureGridSize(row, col);
    _grid[row][col].widget = widget;
    return true;
}

bool TFT_Container::addWidget(TFT_Widget* widget,uint8_t row,uint8_t col,uint16_t marginLeft, uint16_t marginRight
                                                                        ,uint16_t marginTop,uint16_t marginBottom)
{
    ensureGridSize(row, col);
    _grid[row][col].widget = widget;
    _grid[row][col].marginLeft = marginLeft;
    _grid[row][col].marginRight = marginRight;
    _grid[row][col].marginTop = marginTop;
    _grid[row][col].marginBottom = marginBottom;
    return true;
}

void TFT_Container::draw()
{
    for(unsigned int i=0;i<_grid.size();i++)
        for(unsigned int j=0;j<_grid[i].size();j++)
            if(_grid[i][j].widget!=nullptr)
                _grid[i][j].widget->draw();
}

void TFT_Container::setWidgetMargin(uint8_t row, uint8_t col, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
    ensureGridSize(row, col);
    _grid[row][col].marginLeft = left;
    _grid[row][col].marginTop = top;
    _grid[row][col].marginRight = right;
    _grid[row][col].marginBottom = bottom;
}

void TFT_Container::resize(uint16_t newWidth, uint16_t newHeight)
{
    TFT_Rect rect({_rect.x,_rect.y,newWidth,newHeight});
    setRect(rect);
    updateLayout();
}

uint8_t TFT_Container::getNbRow()
{
    return _grid.size();
}

std::vector<uint16_t> TFT_Container::getMaxHeightByRow()
{
    std::vector<uint16_t> maxHeightByRow;
    maxHeightByRow.reserve(_grid.size());

    for (size_t i = 0; i < _grid.size(); i++)
    {
        uint16_t maxH = 0;
        for (size_t j = 0; j < _grid[i].size(); j++)
        {
            if (_grid[i][j].widget)
            {
                uint16_t h = _grid[i][j].widget->getRect().h + _grid[i][j].marginTop + _grid[i][j].marginBottom ;
                if (h > maxH)
                    maxH = h;  
            }
        }
        maxHeightByRow.emplace_back(maxH);
    }
    return maxHeightByRow;
}

void TFT_Container::updateLayout()
{
    uint16_t yOffset = 0;
    uint16_t maxWidth = 0;

    std::vector<uint16_t> maxHeightByRow = getMaxHeightByRow();
    for (size_t i = 0; i < _grid.size(); i++)
    {
        uint16_t xOffset = 0;
        uint16_t rowMaxHeight = maxHeightByRow[i];

        for (size_t j = 0; j < _grid[i].size(); j++)
        {
            TFT_GridElement &el = _grid[i][j];
            TFT_Widget *widget = el.widget;
            if (!widget) continue;

            xOffset += el.marginLeft;
            uint16_t x = _rect.x + xOffset;
            uint16_t y = _rect.y + yOffset + el.marginTop;

            widget->setPos(x, y);

            xOffset += widget->getRect().w + el.marginRight;

            // Suivi de la largeur max atteinte
            uint16_t widgetRight = x + widget->getRect().w + el.marginRight;
            if (widgetRight - _rect.x > maxWidth)
                maxWidth = widgetRight - _rect.x;
        }
        // Avancer à la ligne suivante
        yOffset += rowMaxHeight;
    }
    _rect.w = maxWidth;
    _rect.h = yOffset;
}


void TFT_Container::setPos(uint16_t x, uint16_t y)
{
    _rect.x = x;
    _rect.y = y;
    updateLayout();
}


