#ifndef TFT_SCENEMANAGER_H_INCLUDED
#define TFT_SCENEMANAGER_H_INCLUDED

#include "TFT_Scene.h"

class TFT_SceneManager
{
    public:
    TFT_SceneManager(TFT_Scene *rootScene);
    ~TFT_SceneManager();

    private:
        TFT_Scene  *_root,*_current;

};

TFT_SceneManager::TFT_SceneManager(TFT_Scene *rootScene): _root(rootScene),_current(rootScene)
{
}

TFT_SceneManager::~TFT_SceneManager()
{
}



#endif