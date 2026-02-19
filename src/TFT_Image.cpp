#include "TFT_Image.h"

extern TFT_CoreApp *TFT_CoreAppPtr;

TFT_Image::TFT_Image(const char *path) : _data(nullptr), _colorKey(0),_haveColorKey(false)
{
    if(!load(path))
        _data = nullptr;
}

TFT_Image::TFT_Image(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t *data) : _data(nullptr), _colorKey(0),_haveColorKey(false)
{
    _data = data;
    _rect.x = x;
    _rect.y = y;
    _rect.w = w;
    _rect.h = h;
}
TFT_Image::~TFT_Image()
{
    free(_data);
}

void TFT_Image::setColorKey(uint16_t colorKey)
{
    _haveColorKey = true;
    _colorKey = colorKey;
}

bool TFT_Image::load(const char *path)
{  
    _data =  TFT_CoreAppPtr->openImg(path,&_rect.w,&_rect.h);
    if (_data == nullptr)
        return false;
    return true;
}

void TFT_Image::draw()
{
    if(_haveColorKey)
        TFT_CoreAppPtr->pushImg(_data, _rect,_colorKey);
    else
        TFT_CoreAppPtr->pushImg(_data, _rect);
}

void TFT_Image::resize(uint16_t newWidth, uint16_t newHeight)
{
    if (!_data || _rect.w == 0 || _rect.h == 0) 
        return; // Pas d'image chargée

    uint16_t* newData = (uint16_t*)malloc(newWidth * newHeight * sizeof(uint16_t));
    if (!newData) 
        return; // Allocation échouée

    // Simple scaling nearest neighbor (rapide mais basique)
    for (uint16_t y = 0; y < newHeight; y++)
    {
        uint16_t srcY = y * _rect.h / newHeight;
        for (uint16_t x = 0; x < newWidth; x++)
        {
            uint16_t srcX = x * _rect.w / newWidth;
            newData[y * newWidth + x] = _data[srcY * _rect.w + srcX];
        }
    }

    // Libérer l'ancien buffer
    free(_data);

    // Mettre à jour les données et dimensions
    _data = newData;
    _rect.w = newWidth;
    _rect.h = newHeight;
}
