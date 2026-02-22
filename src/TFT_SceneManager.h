#ifndef TFT_SCENEMANAGER_H
#define TFT_SCENEMANAGER_H

/*          UTILISATION    
/*  Un sens:         
/*  registerLink(&home, Left, &graph);
/*  registerLink(&graph, Left, &stats);
/*  registerLink(&home, Right, &settings);
/*  registerLink(&settings, Right, &info);

/*  Puis l autre:
/*  registerLink(&graph, Right, &home);
/*  registerLink(&stats, Right, &graph);
/*  registerLink(&settings, Left, &home);
/*  registerLink(&info, Left, &settings);
*/

#include <map>
#include <stack>

class TFT_Scene;
enum class TFT_Direction
{
    Up,
    Down,
    Left,
    Right
};

enum class TFT_TransitionType
{
    None,
    SlideLeft,
    SlideRight,
    SlideUp,
    SlideDown,
    Fade
};

struct TFT_SceneLink
{
    TFT_Scene* target = nullptr;
    TFT_TransitionType transition = TFT_TransitionType::None;
};


class TFT_SceneManager
{
public:
    TFT_SceneManager();
    ~TFT_SceneManager();

    void setInitialScene(TFT_Scene* scene);
    void registerLink(TFT_Scene* from,
                      TFT_Direction direction,
                      TFT_Scene* to,
                      TFT_TransitionType transition = TFT_TransitionType::None);

    void navigate(TFT_Direction direction);
    void goTo(TFT_Scene* scene,
              TFT_TransitionType transition = TFT_TransitionType::None);
    void pushScene(TFT_Scene* scene,
                   TFT_TransitionType transition = TFT_TransitionType::None);
    void popScene(TFT_TransitionType transition = TFT_TransitionType::None);

    void update();
    void draw();

    TFT_Scene* getCurrentScene() const;

private:

    void performTransition(TFT_Scene* next,
                           TFT_TransitionType transition);

    TFT_Scene* _currentScene = nullptr;
    std::map<TFT_Scene*, std::map<TFT_Direction, TFT_SceneLink>> _navigationGraph;
    std::stack<TFT_Scene*> _sceneStack;
    bool _isTransitioning = false;
};

void TFT_SceneManager::setInitialScene(TFT_Scene* scene)
{
    if (scene == nullptr)
        return;

    if (_currentScene != nullptr)
        _currentScene->onExit();

    while (!_sceneStack.empty())
        _sceneStack.pop();

    _isTransitioning = false;

    _currentScene = scene;

    _currentScene->onEnter();
}

void TFT_SceneManager::registerLink(TFT_Scene* from,
                                     TFT_Direction direction,
                                     TFT_Scene* to,
                                     TFT_TransitionType transition)
{
    if (from == nullptr || to == nullptr)
        return;

    TFT_SceneLink link;
    link.target = to;
    link.transition = transition;

    _navigationGraph[from][direction] = link;
}

void TFT_SceneManager::navigate(TFT_Direction direction)
{
    if (_currentScene == nullptr || _isTransitioning)
        return;

    auto sceneIt = _navigationGraph.find(_currentScene);
    if (sceneIt == _navigationGraph.end())
        return;

    auto dirIt = sceneIt->second.find(direction);
    if (dirIt == sceneIt->second.end())
        return;

    TFT_SceneLink link = dirIt->second;

    if (link.target == nullptr)
        return;

    performTransition(link.target, link.transition);
}

void TFT_SceneManager::goTo(TFT_Scene* scene,
                            TFT_TransitionType transition)
{
    if (scene == nullptr || _isTransitioning)
        return;

    performTransition(scene, transition);
}

void TFT_SceneManager::pushScene(TFT_Scene* scene,
                                 TFT_TransitionType transition)
{
    if (scene == nullptr || _isTransitioning)
        return;

    if (_currentScene != nullptr)
        _sceneStack.push(_currentScene);

    performTransition(scene, transition);
}

void TFT_SceneManager::popScene(TFT_TransitionType transition)
{
    if (_sceneStack.empty() || _isTransitioning)
        return;

    TFT_Scene* previous = _sceneStack.top();
    _sceneStack.pop();

    performTransition(previous, transition);
}

void TFT_SceneManager::performTransition(TFT_Scene* next,
                                         TFT_TransitionType transition)
{
    if (next == nullptr)
        return;

    _isTransitioning = true;

    if (_currentScene != nullptr)
        _currentScene->onExit();

    //Transition...

    _currentScene = next;

    _currentScene->onEnter();

    _isTransitioning = false;
}
#endif