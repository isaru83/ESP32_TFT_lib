#ifndef TFT_SCENE_H_INCLUDED
#define TFT_SCENE_H_INCLUDED

#include "TFT_CoreApp.h"
#include "TFT_Container.h"

extern TFT_CoreApp *TFT_CoreAppPtr;

class TFT_Scene : public TFT_Container
{
    public:
    TFT_Scene(TFT_Rect rect);
    TFT_Scene(const char *imgPath);
    TFT_Scene(TFT_Rect rect, uint16_t color); 
    void addChild(TFT_Scene *scene);
    void setParent(TFT_Scene *scene);

    virtual void onEnter() {}
    virtual void onExit() {}

    void draw() override;

    private:
        TFT_Image *_bgImg;
        uint16_t _bgClr;
        TFT_Scene *_parent;
        vector<TFT_Scene*> _children;
};
#endif