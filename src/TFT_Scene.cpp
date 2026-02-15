#include "TFT_Scene.h"

TFT_Scene::TFT_Scene(TFT_Rect rect): _bgImg(nullptr),_bgClr(0),_parent(nullptr)
{
     _rect = rect;
}

TFT_Scene::TFT_Scene(const char *imgPath) : _bgImg(nullptr),_bgClr(0),_parent(nullptr)
{
    _bgImg = new TFT_Image(imgPath);
    if(_bgImg!=nullptr)
        _rect = _bgImg->getRect();
}

TFT_Scene::TFT_Scene(TFT_Rect rect,uint16_t color): _bgImg(nullptr),_bgClr(color),_parent(nullptr)
{
    _rect = rect;
} 

void TFT_Scene::draw()
{
    if (_bgImg)
        _bgImg->draw();
    else if(_bgClr != 0)
        TFT_CoreAppPtr->drawRect(_rect,_bgClr);

    TFT_Container::draw();
}

    void TFT_Scene::addChild(TFT_Scene *scene)
    {
        _children.push_back(scene);
    }
    void TFT_Scene::setParent(TFT_Scene *scene)
    {
        _parent = scene;
    }